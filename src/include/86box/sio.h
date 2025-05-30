/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Definitions for the Super I/O chips.
 *
 * Authors: Fred N. van Kempen, <decwiz@yahoo.com>
 *
 *          Copyright 2017-2020 Fred N. van Kempen.
 */

#ifndef EMU_SIO_H
#define EMU_SIO_H

/* ACC Micro */
extern const device_t acc3221_device;

/* Acer / ALi */
extern const device_t ali5105_device;

extern const device_t ali5123_device;

/* Chips & Technologies */
extern const device_t f82c606_device;

extern const device_t f82c710_device;

/* SM(S)C */
extern const device_t fdc37c651_device;
extern const device_t fdc37c651_ide_device;
extern const device_t fdc37c661_device;
extern const device_t fdc37c661_ide_device;
extern const device_t fdc37c661_ide_sec_device;
extern const device_t fdc37c663_device;
extern const device_t fdc37c663_ide_device;
extern const device_t fdc37c665_device;
extern const device_t fdc37c665_ide_device;
extern const device_t fdc37c665_ide_pri_device;
extern const device_t fdc37c665_ide_sec_device;
extern const device_t fdc37c666_device;

extern const device_t fdc37c669_device;
extern const device_t fdc37c669_370_device;

extern const device_t fdc37c67x_device;

extern const device_t fdc37c931apm_device;
extern const device_t fdc37c931apm_compaq_device;
extern const device_t fdc37c932fr_device;
extern const device_t fdc37c932qf_device;
extern const device_t fdc37c932_device;
extern const device_t fdc37c935_device;
extern const device_t fdc37c935_370_device;
extern const device_t fdc37c935_no_nvr_device;

extern const device_t fdc37m60x_device;
extern const device_t fdc37m60x_370_device;

/* ITE */
extern const device_t it8661f_device;
extern const device_t it8671f_device;
extern const device_t i82091aa_device;
extern const device_t i82091aa_398_device;
extern const device_t i82091aa_ide_pri_device;
extern const device_t i82091aa_ide_device;

/* ITE IT8702 */
extern const device_t it8702_device;

/* National Semiconductor NSC366 (PC87366) */
extern const device_t nsc366_device;
extern const device_t nsc366_4f_device;

/* National Semiconductors */
extern const device_t pc87310_device;
extern const device_t pc87310_ide_device;

extern const device_t pc87306_device;
extern const device_t pc87311_device;
extern const device_t pc87311_ide_device;
extern const device_t pc87332_device;
extern const device_t pc87332_398_device;
extern const device_t pc87332_398_ide_device;
extern const device_t pc87332_398_ide_sec_device;
extern const device_t pc87332_398_ide_fdcon_device;

extern const device_t pc87307_device;
extern const device_t pc87307_15c_device;
extern const device_t pc87307_both_device;
extern const device_t pc97307_device;

extern const device_t pc87309_device;
extern const device_t pc87309_15c_device;

/* LG Prime */
extern const device_t prime3b_device;
extern const device_t prime3b_ide_device;
extern const device_t prime3c_device;
extern const device_t prime3c_ide_device;

/* IBM PS/1 */
extern const device_t ps1_m2133_sio;

/* Super I/O Detect */
#ifdef USE_SIO_DETECT
extern const device_t sio_detect_device;
#endif /* USE_SIO_DETECT */

/* UMC */
extern const device_t um8663af_device;
extern const device_t um8663af_ide_device;
extern const device_t um8663af_sec_device;
extern const device_t um8663bf_device;
extern const device_t um8663bf_ide_device;
extern const device_t um8663bf_sec_device;

extern const device_t um8669f_device;
extern const device_t um8669f_ide_device;
extern const device_t um8669f_ide_sec_device;

/* VIA */
extern void vt82c686_sio_write(uint8_t addr, uint8_t val, void *priv);

extern const device_t via_vt82c686_sio_device;

/* VLSI */
extern const device_t vl82c113_device;

/* Winbond */
extern const device_t w83627hf_device;
extern const device_t w83627hf_no_hwm_device;
extern const device_t w83787f_88h_device;
extern const device_t w83787f_device;
extern const device_t w83787f_ide_device;
extern const device_t w83787f_ide_en_device;
extern const device_t w83787f_ide_sec_device;

extern const device_t w83877f_device;
extern const device_t w83877f_president_device;
extern const device_t w83877tf_device;
extern const device_t w83877tf_acorp_device;

#define TYPE_W83977EF    0x52F0
#define TYPE_W83977F     0x9771
#define TYPE_W83977TF    0x9773
#define TYPE_W83977ATF   0x9774

extern const device_t w83977f_device;
extern const device_t w83977f_370_device;
extern const device_t w83977tf_device;
extern const device_t w83977ef_device;
extern const device_t w83977ef_370_device;

#endif /*EMU_SIO_H*/
