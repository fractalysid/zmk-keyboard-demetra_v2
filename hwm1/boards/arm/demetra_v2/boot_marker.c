/*
 * Copyright (c) 2026 Mattia Buscema
 * SPDX-License-Identifier: MIT
 *
 * Boot marker per bring-up Demetra v2 — tracer del boot via LED P0.15
 * (attivo basso, alimentato da VDD) quando non c'è un debugger collegato.
 *
 * Sequenza attesa a boot riuscito:
 *   1. lampo LUNGO 400ms   = init EARLY eseguita (l'app parte: handoff OK)
 *   2. due lampi brevi     = POST_KERNEL raggiunto (kernel + driver su)
 *   3. LED fisso acceso    = livello APPLICATION raggiunto
 *
 * Decodifica dei fallimenti:
 *   - nessun lampo lungo            → l'app non parte mai: handoff/bootloader
 *   - solo lampo lungo, poi buio    → crash tra reset e POST_KERNEL
 *   - LED inchiodato acceso ai lampi brevi → k_busy_wait ferma: LFCLK/timer morto
 *   - lampi ok ma mai fisso acceso  → crash prima di APPLICATION (init ZMK)
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>

#define MARKER_PIN 15

#define P0_OUTSET (*(volatile uint32_t *)0x50000508UL)
#define P0_OUTCLR (*(volatile uint32_t *)0x5000050CUL)
#define P0_DIRSET (*(volatile uint32_t *)0x50000518UL)

static inline void marker_led(int on)
{
	if (on) {
		P0_OUTCLR = 1UL << MARKER_PIN; /* attivo basso */
	} else {
		P0_OUTSET = 1UL << MARKER_PIN;
	}
}

/* Ritardo a vuoto: gira prima di kernel e clock (~64 MHz, ~4 cicli/iter). */
static void marker_spin_ms(uint32_t ms)
{
	for (volatile uint32_t i = 0; i < ms * 16000UL; i++) {
	}
}

/* Fase 1 — livello EARLY: primo codice C dopo z_cstart, prima del kernel.
 * (z_arm_platform_init non è sovrascrivibile su nRF: lo definisce il SoC.) */
static int marker_early(void)
{
	P0_OUTSET = 1UL << MARKER_PIN;
	P0_DIRSET = 1UL << MARKER_PIN;

	marker_led(1);
	marker_spin_ms(400);
	marker_led(0);
	marker_spin_ms(150);
	return 0;
}
SYS_INIT(marker_early, EARLY, 0);

/* Fase 2 — kernel e driver inizializzati. k_busy_wait dipende dal timer di
 * sistema (RTC su LFCLK): se si blocca qui, è il LFCLK l'indiziato. */
static int marker_post_kernel(void)
{
	for (int i = 0; i < 2; i++) {
		marker_led(1);
		k_busy_wait(150000);
		marker_led(0);
		k_busy_wait(150000);
	}
	return 0;
}
SYS_INIT(marker_post_kernel, POST_KERNEL, 99);

/* Fase 3 — fine della catena di init: LED fisso acceso. */
static int marker_application(void)
{
	marker_led(1);
	return 0;
}
SYS_INIT(marker_application, APPLICATION, 99);
