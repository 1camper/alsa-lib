/****************************************************************************
 *                                                                          *
 *                               mixer.h                                    *
 *                           Mixer Interface                                *
 *                                                                          *
 ****************************************************************************/

typedef struct _snd_mixer snd_mixer_t;
typedef struct _snd_mixer_class snd_mixer_class_t;
typedef struct _snd_mixer_info snd_mixer_info_t;
typedef struct _snd_mixer_elem snd_mixer_elem_t;
typedef int (*snd_mixer_callback_t)(snd_mixer_t *ctl,
				    unsigned int mask,
				    snd_mixer_elem_t *elem);
typedef int (*snd_mixer_elem_callback_t)(snd_mixer_elem_t *elem,
					 unsigned int mask);
typedef int (*snd_mixer_compare_t)(const snd_mixer_elem_t *e1,
				   const snd_mixer_elem_t *e2);

enum _snd_mixer_elem_type {
	SND_MIXER_ELEM_SIMPLE,
	SND_MIXER_ELEM_LAST = SND_MIXER_ELEM_SIMPLE,
};

#ifdef SND_ENUM_TYPECHECK
typedef struct __snd_mixer_elem_type *snd_mixer_elem_type_t;
#else
typedef enum _snd_mixer_elem_type snd_mixer_elem_type_t;
#endif

#define SND_MIXER_ELEM_SIMPLE ((snd_mixer_elem_type_t) SND_MIXER_ELEM_SIMPLE)
#define SND_MIXER_ELEM_LAST ((snd_mixer_elem_type_t) SND_MIXER_ELEM_LAST)


#ifdef __cplusplus
extern "C" {
#endif

int snd_mixer_open(snd_mixer_t **mixer);
int snd_mixer_close(snd_mixer_t *mixer);
int snd_mixer_info(snd_mixer_t *mixer, snd_mixer_info_t *info);
snd_mixer_elem_t *snd_mixer_first_elem(snd_mixer_t *mixer);
snd_mixer_elem_t *snd_mixer_last_elem(snd_mixer_t *mixer);
int snd_mixer_handle_events(snd_mixer_t *mixer);
int snd_mixer_attach(snd_mixer_t *mixer, const char *name);
int snd_mixer_detach(snd_mixer_t *mixer, const char *name);
int snd_mixer_poll_descriptors_count(snd_mixer_t *mixer);
int snd_mixer_poll_descriptors(snd_mixer_t *mixer, struct pollfd *pfds, unsigned int space);
int snd_mixer_load(snd_mixer_t *mixer);
int snd_mixer_wait(snd_mixer_t *mixer, int timeout);
int snd_mixer_set_compare(snd_mixer_t *mixer, snd_mixer_compare_t msort);

snd_mixer_elem_t *snd_mixer_elem_next(snd_mixer_elem_t *elem);
snd_mixer_elem_t *snd_mixer_elem_prev(snd_mixer_elem_t *elem);

int snd_mixer_class_unregister(snd_mixer_class_t *clss);

#ifdef __cplusplus
}
#endif

/*
 *  Simple (legacy) mixer API
 */

enum _snd_mixer_selem_channel_id {
	SND_MIXER_SCHN_UNKNOWN = -1,
	SND_MIXER_SCHN_FRONT_LEFT = 0,
	SND_MIXER_SCHN_FRONT_RIGHT,
	SND_MIXER_SCHN_FRONT_CENTER,
	SND_MIXER_SCHN_REAR_LEFT,
	SND_MIXER_SCHN_REAR_RIGHT,
	SND_MIXER_SCHN_WOOFER,
	SND_MIXER_SCHN_LAST = 31,
	SND_MIXER_SCHN_MONO = SND_MIXER_SCHN_FRONT_LEFT
};

/* Simple mixer */

typedef struct _snd_mixer_selem_id snd_mixer_selem_id_t;

#ifdef SND_ENUM_TYPECHECK
typedef struct __snd_mixer_selem_channel_id *snd_mixer_selem_channel_id_t;
#else
typedef enum _snd_mixer_selem_channel_id snd_mixer_selem_channel_id_t;
#endif

#define SND_MIXER_SCHN_UNKNOWN ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_UNKNOWN)
#define SND_MIXER_SCHN_FRONT_LEFT ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_FRONT_LEFT)
#define SND_MIXER_SCHN_FRONT_RIGHT ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_FRONT_RIGHT)
#define SND_MIXER_SCHN_FRONT_CENTER ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_FRONT_CENTER)
#define SND_MIXER_SCHN_REAR_LEFT ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_REAR_LEFT)
#define SND_MIXER_SCHN_REAR_RIGHT ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_REAR_RIGHT)
#define SND_MIXER_SCHN_WOOFER ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_WOOFER)
#define SND_MIXER_SCHN_LAST ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_LAST)
#define SND_MIXER_SCHN_MONO ((snd_mixer_selem_channel_id_t) SND_MIXER_SCHN_MONO)

#ifdef __cplusplus
extern "C" {
#endif

const char *snd_mixer_selem_channel_name(snd_mixer_selem_channel_id_t channel);

int snd_mixer_selem_register(snd_mixer_t *mixer, void *arg,
			     snd_mixer_class_t **classp);
void snd_mixer_selem_get_id(snd_mixer_elem_t *element,
			    snd_mixer_selem_id_t *id);
const char *snd_mixer_selem_get_name(snd_mixer_elem_t *elem);
unsigned int snd_mixer_selem_get_index(snd_mixer_elem_t *elem);
snd_mixer_elem_t *snd_mixer_find_selem(snd_mixer_t *mixer,
				       const snd_mixer_selem_id_t *id);

int snd_mixer_selem_is_playback_mono(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_playback_channel(snd_mixer_elem_t *obj, snd_mixer_selem_channel_id_t channel);
int snd_mixer_selem_get_playback_min(snd_mixer_elem_t *elem);
int snd_mixer_selem_get_playback_max(snd_mixer_elem_t *elem);
int snd_mixer_selem_is_capture_mono(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_capture_channel(snd_mixer_elem_t *obj, snd_mixer_selem_channel_id_t channel);
int snd_mixer_selem_get_capture_min(snd_mixer_elem_t *elem);
int snd_mixer_selem_get_capture_max(snd_mixer_elem_t *elem);
int snd_mixer_selem_get_capture_group(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_common_volume(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_playback_volume(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_playback_volume_joined(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_capture_volume(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_capture_volume_joined(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_common_switch(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_playback_switch(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_playback_switch_joined(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_capture_switch(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_capture_switch_joined(snd_mixer_elem_t *elem);
int snd_mixer_selem_has_capture_switch_exclusive(snd_mixer_elem_t *elem);

int snd_mixer_selem_get_playback_volume(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, long *value);
int snd_mixer_selem_get_capture_volume(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, long *value);
int snd_mixer_selem_get_playback_switch(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, int *value);
int snd_mixer_selem_get_capture_switch(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, int *value);
int snd_mixer_selem_set_playback_volume(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, long value);
int snd_mixer_selem_set_capture_volume(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, long value);
int snd_mixer_selem_set_playback_volume_all(snd_mixer_elem_t *elem, long value);
int snd_mixer_selem_set_capture_volume_all(snd_mixer_elem_t *elem, long value);
int snd_mixer_selem_set_playback_switch(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, int value);
int snd_mixer_selem_set_capture_switch(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, int value);
int snd_mixer_selem_set_playback_switch_all(snd_mixer_elem_t *elem, int value);
int snd_mixer_selem_set_capture_switch_all(snd_mixer_elem_t *elem, int value);
void snd_mixer_selem_set_playback_volume_range(snd_mixer_elem_t *elem, 
					       long min, long max);
void snd_mixer_selem_set_capture_volume_range(snd_mixer_elem_t *elem, 
					      long min, long max);
#ifdef __cplusplus
}
#endif

