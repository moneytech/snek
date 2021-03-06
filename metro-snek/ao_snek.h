/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef _AO_SNEK_H_
#define _AO_SNEK_H_

#include <ao.h>

int
snek_eeprom_getchar(FILE *stream);

uint16_t
ao_snek_get_adc(void *gpio, uint8_t pin, uint8_t adc);

void
ao_snek_port_init(void);

void
ao_snek(void);

void
ao_snek_reset(void);

void
ao_snek_running(bool running);

#define SNEK_CODE_HOOK_START	ao_snek_running(true);
#define SNEK_CODE_HOOK_STOP	ao_snek_running(false);

#endif /* _AO_SNEK_H_ */
