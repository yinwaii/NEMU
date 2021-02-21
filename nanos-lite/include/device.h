#ifndef __DEVICE_H__
#define __DEVICE_H__
#include <common.h>

#define CONCAT3(a, b, c) a ## b ## c
#define reg_map_read(f)                                          \
	f(TIMER_CONFIG) f(TIMER_RTC) f(TIMER_UPTIME) f(INPUT_CONFIG) \
		f(INPUT_KEYBRD) f(GPU_CONFIG) f(GPU_STATUS)              \
			f(UART_CONFIG) f(AUDIO_CONFIG) f(AUDIO_STATUS)       \
				f(DISK_CONFIG) f(NET_CONFIG)
#define reg_map_write(f) \
	f(GPU_FBDRAW) f(AUDIO_CTRL) f(AUDIO_PLAY)

#define make_declare_read(name) \
	size_t CONCAT3(AM_, name, _read)(void *buf, size_t offset, size_t len);
#define make_declare_write(name) \
	size_t CONCAT3(AM_, name, _write)(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
reg_map_read(make_declare_read)
reg_map_write(make_declare_write)

#endif