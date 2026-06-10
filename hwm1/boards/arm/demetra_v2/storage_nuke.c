/*
 * Copyright (c) 2026 Mattia Buscema
 * SPDX-License-Identifier: MIT
 *
 * Eraser one-shot della storage partition (NVS, 0xEC000-0x8000) per bring-up,
 * strumentato per localizzare anche il punto di morte. Solo delay a spin:
 * nessuna dipendenza dal timer di sistema.
 *
 * Pattern LED (P0.15, attivo basso), dopo i marker di boot_marker.c:
 *   3 lampi rapidi          = raggiunto APPLICATION,0 (pre-erase)
 *   LED fisso acceso        = erase IN CORSO (se resta fisso: erase bloccata)
 *   lampeggio lento perpetuo = erase COMPLETATA → riflashare firmware normale
 *   buio dopo i 3 lampi     = flash_area_open fallita
 *
 * Build:
 *   kb-build.sh --hw zmk-keyboard-demetra_v2 Cotechino_34 demetra_v2_left \
 *       nuke_left -- -DCONFIG_DEMETRA_V2_STORAGE_NUKE=y
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>

#define NUKE_PIN 15

#define P0_OUTSET (*(volatile uint32_t *)0x50000508UL)
#define P0_OUTCLR (*(volatile uint32_t *)0x5000050CUL)
#define P0_DIRSET (*(volatile uint32_t *)0x50000518UL)

static inline void nuke_led(int on)
{
	if (on) {
		P0_OUTCLR = 1UL << NUKE_PIN;
	} else {
		P0_OUTSET = 1UL << NUKE_PIN;
	}
}

static void nuke_spin_ms(uint32_t ms)
{
	for (volatile uint32_t i = 0; i < ms * 16000UL; i++) {
	}
}

static int storage_nuke(void)
{
	const struct flash_area *fa;
	int err;

	P0_OUTSET = 1UL << NUKE_PIN;
	P0_DIRSET = 1UL << NUKE_PIN;

	/* 3 lampi rapidi: siamo vivi a APPLICATION,0 */
	for (int i = 0; i < 3; i++) {
		nuke_led(1);
		nuke_spin_ms(80);
		nuke_led(0);
		nuke_spin_ms(80);
	}

	err = flash_area_open(FIXED_PARTITION_ID(storage_partition), &fa);
	if (err != 0) {
		for (;;) { /* buio = open fallita */
		}
	}

	nuke_led(1); /* fisso acceso = erase in corso */
	err = flash_area_erase(fa, 0, fa->fa_size);
	flash_area_close(fa);
	nuke_led(0);

	if (err != 0) {
		for (;;) { /* due lampi lenti poi pausa lunga = erase fallita con errore */
			nuke_led(1); nuke_spin_ms(150); nuke_led(0); nuke_spin_ms(150);
			nuke_led(1); nuke_spin_ms(150); nuke_led(0); nuke_spin_ms(1500);
		}
	}

	for (;;) { /* lampeggio lento perpetuo = erase completata */
		nuke_led(1);
		nuke_spin_ms(500);
		nuke_led(0);
		nuke_spin_ms(500);
	}
	return 0;
}
SYS_INIT(storage_nuke, APPLICATION, 0);
