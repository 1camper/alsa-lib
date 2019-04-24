/**
 * \file pcm/pcm_dmix_x86_64.h
 * \ingroup PCM_Plugins
 * \brief PCM Direct Stream Mixing (dmix) Plugin Interface - X86-64 assembler code
 * \author Takashi Iwai <tiwai@suse.de>
 * \date 2003
 */
/*
 *  PCM - Direct Stream Mixing
 *  Copyright (c) 2003 by Jaroslav Kysela <perex@perex.cz>
 *                        Takashi Iwai <tiwai@suse.de>
 *
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation; either version 2.1 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 *  MMX optimized
 */
static void MIX_AREAS_16(unsigned int size,
			 volatile signed short *dst, signed short *src,
			 volatile signed int *sum, size_t dst_step,
			 size_t src_step, size_t sum_step)
{
	int eax, ecx, edx;

	__asm__ __volatile__ (
		"\n"

		/*
		 * while (size-- > 0) {
		 */
		"\ttestl %[size], %[size]\n"
		"jz 6f\n"

		"\t.p2align 4,,15\n"

		"1:"

		/*
		 *   sample = *src;
		 *   sum_sample = *sum;
		 *   if (cmpxchg(*dst, 0, 1) == 0)
		 *     sample -= sum_sample;
		 *   xadd(*sum, sample);
		 */
		"\txorl %[eax], %[eax]\n"
		"\tmovl $1, %[ecx]\n"
		"\tmovl (%q[sum]), %[edx]\n"
		"\t" LOCK_PREFIX "cmpxchgw %w[ecx], (%q[dst])\n"
		"\tmovswl (%q[src]), %[ecx]\n"
		"\tjnz 2f\n"
		"\t" XSUB " %[edx], %[ecx]\n"
		"2:"
		"\t" LOCK_PREFIX XADD " %[ecx], (%q[sum])\n"

		/*
		 *   do {
		 *     sample = old_sample = *sum;
		 *     saturate(v);
		 *     *dst = sample;
		 *   } while (v != *sum);
		 */

		"3:"
		"\tmovl (%q[sum]), %[ecx]\n"
		"\tmovd %[ecx], %%mm0\n"
		"\tpackssdw %%mm0, %%mm0\n"
		"\tmovd %%mm0, %[eax]\n"
		"\tmovw %w[eax], (%q[dst])\n"
		"\tcmpl %[ecx], (%q[sum])\n"
		"\tjnz 3b\n"

		/*
		 * while (size-- > 0)
		 */
		"\tadd %[dst_step], %[dst]\n"
		"\tadd %[src_step], %[src]\n"
		"\tadd %[sum_step], %[sum]\n"
		"\tdecl %[size]\n"
		"\tjnz 1b\n"

		"6:"
		
		"\temms\n"

		: [eax] "=&a" (eax), [ecx] "=&c" (ecx), [edx] "=&d" (edx)
		: [dst] "r" (dst), [src] "r" (src), [sum] "r" (sum), [size] "r" (size),
		  [dst_step] "r" (dst_step), [src_step] "r" (src_step), [sum_step] "r" (sum_step)
	);
}

/*
 *  32-bit version (24-bit resolution)
 */
static void MIX_AREAS_32(unsigned int size,
			 volatile signed int *dst, signed int *src,
			 volatile signed int *sum, size_t dst_step,
			 size_t src_step, size_t sum_step)
{
	int eax, ecx, edx;

	__asm__ __volatile__ (
		"\n"

		/*
		 * while (size-- > 0) {
		 */
		"\ttestl %[size], %[size]\n"
		"jz 6f\n"

		"\t.p2align 4,,15\n"

		"1:"

		/*
		 *   sample = *src;
		 *   sum_sample = *sum;
		 *   if (cmpxchg(*dst, 0, 1) == 0)
		 *     sample -= sum_sample;
		 *   xadd(*sum, sample);
		 */
		"\txorl %[eax], %[eax]\n"
		"\tmovl $1, %[ecx]\n"
		"\tmovl (%q[sum]), %[edx]\n"
		"\t" LOCK_PREFIX "cmpxchgl %[ecx], (%q[dst])\n"
		"\tjnz 2f\n"
		"\tmovl (%q[src]), %[ecx]\n"
		/* sample >>= 8 */
		"\tsarl $8, %[ecx]\n"
		"\t" XSUB " %[edx], %[ecx]\n"
		"\tjmp 21f\n"
		"2:"
		"\tmovl (%q[src]), %[ecx]\n"
		/* sample >>= 8 */
		"\tsarl $8, %[ecx]\n"
		"21:"
		"\t" LOCK_PREFIX XADD " %[ecx], (%q[sum])\n"

		/*
		 *   do {
		 *     sample = old_sample = *sum;
		 *     saturate(v);
		 *     *dst = sample;
		 *   } while (v != *sum);
		 */

		"3:"
		"\tmovl (%q[sum]), %[ecx]\n"
		/*
		 *  if (sample > 0x7fff00)
		 */
		"\tmovl $0x7fffff, %[eax]\n"
		"\tcmpl %[eax], %[ecx]\n"
		"\tjg 4f\n"
		/*
		 *  if (sample < -0x800000)
		 */
		"\tmovl $-0x800000, %[eax]\n"
		"\tcmpl %[eax], %[ecx]\n"
		"\tjl 4f\n"
		"\tmovl %[ecx], %[eax]\n"
		"4:"
		/*
		 *  sample <<= 8;
		 */
		"\tsall $8, %[eax]\n"
		"\tmovl %[eax], (%q[dst])\n"
		"\tcmpl %[ecx], (%q[sum])\n"
		"\tjnz 3b\n"

		/*
		 * while (size-- > 0)
		 */
		"\tadd %[dst_step], %[dst]\n"
		"\tadd %[src_step], %[src]\n"
		"\tadd %[sum_step], %[sum]\n"
		"\tdecl %[size]\n"
		"\tjnz 1b\n"
		
		"6:"

		: [eax] "=&a" (eax), [ecx] "=&c" (ecx), [edx] "=&d" (edx)
		: [dst] "r" (dst), [src] "r" (src), [sum] "r" (sum), [size] "r" (size),
		  [dst_step] "r" (dst_step), [src_step] "r" (src_step), [sum_step] "r" (sum_step)
	);
}

/*
 *  24-bit version
 */
static void MIX_AREAS_24(unsigned int size,
			 volatile unsigned char *dst, unsigned char *src,
			 volatile signed int *sum, size_t dst_step,
			 size_t src_step, size_t sum_step)
{
	int eax, ecx, edx;

	__asm__ __volatile__ (
		"\n"

		/*
		 * while (size-- > 0) {
		 */
		"\ttestl %[size], %[size]\n"
		"jz 6f\n"

		"\t.p2align 4,,15\n"

		"1:"

		/*
		 *   sample = *src;
		 *   sum_sample = *sum;
		 *   if (test_and_set_bit(0, dst) == 0)
		 *     sample -= sum_sample;
		 *   *sum += sample;
		 */
		"\tmovsbl 2(%q[src]), %[eax]\n"
		"\tmovzwl (%q[src]), %[ecx]\n"
		"\tmovl (%q[sum]), %[edx]\n"
		"\tsall $16, %[eax]\n"
		"\torl %[eax], %[ecx]\n"
		"\t" LOCK_PREFIX "btsw $0, (%q[dst])\n"
		"\tjc 2f\n"
		"\t" XSUB " %[edx], %[ecx]\n"
		"2:"
		"\t" LOCK_PREFIX XADD " %[ecx], (%q[sum])\n"

		/*
		 *   do {
		 *     sample = old_sample = *sum;
		 *     saturate(sample);
		 *     *dst = sample | 1;
		 *   } while (old_sample != *sum);
		 */

		"3:"
		"\tmovl (%q[sum]), %[ecx]\n"

		"\tmovl $0x7fffff, %[eax]\n"
		"\tmovl $-0x7fffff, %[edx]\n"
		"\tcmpl %[eax], %[ecx]\n"
		"\tcmovng %[ecx], %[eax]\n"
		"\tcmpl %[edx], %[ecx]\n"
		"\tcmovl %[edx], %[eax]\n"

		"\torl $1, %[eax]\n"
		"\tmovw %w[eax], (%q[dst])\n"
		"\tshrl $16, %[eax]\n"
		"\tmovb %b[eax], 2(%q[dst])\n"
	
		"\tcmpl %[ecx], (%q[sum])\n"
		"\tjnz 3b\n"

		/*
		 * while (size-- > 0)
		 */
		"\tadd %[dst_step], %[dst]\n"
		"\tadd %[src_step], %[src]\n"
		"\tadd %[sum_step], %[sum]\n"
		"\tdecl %[size]\n"
		"\tjnz 1b\n"
		
		"6:"

		: [eax] "=&a" (eax), [ecx] "=&c" (ecx), [edx] "=&d" (edx)
		: [dst] "r" (dst), [src] "r" (src), [sum] "r" (sum), [size] "r" (size),
		  [dst_step] "r" (dst_step), [src_step] "r" (src_step), [sum_step] "r" (sum_step)
	);
}
