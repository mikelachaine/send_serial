/***********************************************************************
 *  ftx1_cat_full.h
 *
 *  Complete set of function prototypes for the Yaesu FTX‑1 CAT protocol
 *  (exactly the commands documented in the PDF you supplied).
 *
 *  Design notes
 *  ------------
 *  • All functions return 0 on success, a negative errno‑style value on error.
 *  • The first parameter of every function is the opened serial‑port file
 *    descriptor (e.g. the result of open("/dev/ttyUSB0", O_RDWR|O_NOCTTY)).
 *  • Numeric arguments are expressed in the most convenient unit
 *    (frequency in MHz, gain 0‑100, etc.).
 *  • For each command that has both a “set” and a “read” form three prototypes
 *    are generated:
 *        – cat_<cmd>_set(...)
 *        – cat_<cmd>_get(...)
 *        – cat_<cmd>_parse(...)   // optional helper to turn the raw reply into a C value
 *  • The low‑level routine `cat_send_raw()` builds the ASCII command,
 *    transmits it, and (if requested) reads the answer up to the terminating ‘;’.
 *
 *  Author: Lumo (Proton AI) – July 2025
 ***********************************************************************/

#ifndef FTX1_CAT_FULL_H
#define FTX1_CAT_FULL_H

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
 *  Low‑level helper (identical to the one in the shorter header)
 * -------------------------------------------------------------------- */
int cat_send_raw(int fd,
                 const char *cmd,          /* command without trailing ';' */
                 int expect_reply,         /* 0 = fire‑and‑forget, 1 = read answer */
                 char *reply_buf,
                 size_t reply_len);

/* --------------------------------------------------------------------
 *  Helper structures used by several commands
 * -------------------------------------------------------------------- */
typedef struct {
    long  raw;          /* raw integer value as transmitted */
    char  txt[32];      /* original ASCII representation (null‑terminated) */
} cat_raw_reply_t;

/* --------------------------------------------------------------------
 *  Enumerations
 * -------------------------------------------------------------------- */
typedef enum {
    CAT_MODE_LSB = 0,
    CAT_MODE_USB = 1,
    CAT_MODE_CW  = 2,
    CAT_MODE_FM  = 3,
    CAT_MODE_AM  = 4,
    CAT_MODE_DATA= 5,
    CAT_MODE_RTTY= 6,
    CAT_MODE_NONE = -1
} cat_mode_t;

/* --------------------------------------------------------------------
 *  1. Frequency commands
 * -------------------------------------------------------------------- */

/* Main‑side VFO (FA) */
int cat_set_frequency_main(int fd, double mhz);          /* FAxxxxxxxxx; */
int cat_get_frequency_main(int fd, double *mhz);         /* FA; */
int cat_parse_frequency(const char *reply, double *mhz);

/* Sub‑side VFO (FB) */
int cat_set_frequency_sub(int fd, double mhz);           /* FBxxxxxxxxx; */
int cat_get_frequency_sub(int fd, double *mhz);          /* FB; */
int cat_parse_frequency_sub(const char *reply, double *mhz);

/* Transmit (TX) VFO – not always present, but listed in the manual (FT) */
int cat_set_frequency_tx(int fd, double mhz);            /* FTxxxxxxxxx; */
int cat_get_frequency_tx(int fd, double *mhz);           /* FT; */
int cat_parse_frequency_tx(const char *reply, double *mhz);

/* --------------------------------------------------------------------
 *  2. Mode commands (MD)
 * -------------------------------------------------------------------- */
int cat_set_mode(int fd, cat_mode_t mode);               /* MDn; */
int cat_get_mode(int fd, cat_mode_t *mode);              /* MD; */
int cat_parse_mode(const char *reply, cat_mode_t *mode);

/* --------------------------------------------------------------------
 *  3. VFO / Memory handling
 * -------------------------------------------------------------------- */

/* VM – store/recall memory channel (VM0nn / VM1nn) */
int cat_save_memory(int fd, int mem_no);                  /* VM0nn; */
int cat_recall_memory(int fd, int mem_no);               /* VM1nn; */
int cat_parse_memory(const char *reply, int *mem_no);

/* VS – VFO select (0=MAIN,1=SUB‑RX,2=SUB‑TX) */
int cat_set_vfo_select(int fd, int sel);                 /* VSn;   (0‑2) */
int cat_get_vfo_select(int fd, int *sel);                /* VS;    */
int cat_parse_vfo_select(const char *reply, int *sel);

/* --------------------------------------------------------------------
 *  4. Split operation (ST)
 * -------------------------------------------------------------------- */
int cat_set_split(int fd, int on);                       /* STn;   (0=off,1=on) */
int cat_get_split(int fd, int *on);                      /* ST;    */
int cat_parse_split(const char *reply, int *on);

/* --------------------------------------------------------------------
 *  5. Transmit (PTT) control – TX
 * -------------------------------------------------------------------- */
int cat_set_tx(int fd, int state);                       /* TXn;   (0‑2) */
int cat_get_tx(int fd, int *state);                      /* TX;    */
int cat_parse_tx(const char *reply, int *state);

/* --------------------------------------------------------------------
 *  6. VOX handling
 * -------------------------------------------------------------------- */

/* VX – VOX on/off */
int cat_set_vox(int fd, int on);                         /* VXn;   (0=off,1=on) */
int cat_get_vox(int fd, int *on);                        /* VX;    */
int cat_parse_vox(const char *reply, int *on);

/* VG – VOX gain (0‑100) */
int cat_set_vox_gain(int fd, int gain);                  /* VGnn;  (0‑100) */
int cat_get_vox_gain(int fd, int *gain);                 /* VG;    */
int cat_parse_vox_gain(const char *reply, int *gain);

/* --------------------------------------------------------------------
 *  7. Zero‑In (ZI)
 * -------------------------------------------------------------------- */
int cat_set_zero_in(int fd, int side);                   /* ZI P1;   (0=MAIN,1=SUB) */
int cat_get_zero_in(int fd, int *side);                  /* ZI;      */
int cat_parse_zero_in(const char *reply, int *side);

/* --------------------------------------------------------------------
 *  8. Firmware version (VE)
 * -------------------------------------------------------------------- */
int cat_get_firmware_version(int fd, char *buf, size_t buflen);   /* VE; */
int cat_parse_firmware_version(const char *reply, char *out, size_t outlen);

/* --------------------------------------------------------------------
 *  9. Antenna tuner (AT) – optional on some FTX‑1 firmware versions
 * -------------------------------------------------------------------- */
int cat_set_antenna_tuner(int fd, int on);               /* ATn;   (0=off,1=on) */
int cat_get_antenna_tuner(int fd, int *on);              /* AT;    */
int cat_parse_antenna_tuner(const char *reply, int *on);

/* --------------------------------------------------------------------
 * 10. Notch filter (NF) – enable/disable
 * -------------------------------------------------------------------- */
int cat_set_notch_filter(int fd, int on);                /* NF n;  (0=off,1=on) */
int cat_get_notch_filter(int fd, int *on);               /* NF;    */
int cat_parse_notch_filter(const char *reply, int *on);

/* --------------------------------------------------------------------
 * 11. Power output (PO) – read‑only
 * -------------------------------------------------------------------- */
int cat_get_power_output(int fd, double *watts);         /* PO;    */
int cat_parse_power_output(const char *reply, double *watts);

/* --------------------------------------------------------------------
 * 12. SWR meter (SW) – read‑only
 * -------------------------------------------------------------------- */
int cat_get_swr(int fd, double *swr);                    /* SW;    */
int cat_parse_swr(const char *reply, double *swr);

/* --------------------------------------------------------------------
 * 13. Temperature (TP) – read‑only
 * -------------------------------------------------------------------- */
int cat_get_temperature(int fd, double *celsius);        /* TP;    */
int cat_parse_temperature(const char *reply, double *celsius);

/* --------------------------------------------------------------------
 * 14. Battery voltage (BV) – read‑only
 * -------------------------------------------------------------------- */
int cat_get_battery_voltage(int fd, double *volts);      /* BV;    */
int cat_parse_battery_voltage(const char *reply, double *volts);

/* --------------------------------------------------------------------
 * 15. RF power level (RF) – set/read (percentage)
 * -------------------------------------------------------------------- */
int cat_set_rf_power(int fd, int percent);               /* RFnn;  (0‑100) */
int cat_get_rf_power(int fd, int *percent);              /* RF;    */
int cat_parse_rf_power(const char *reply, int *percent);

/* --------------------------------------------------------------------
 * 16. Mic gain (MG) – set/read (0‑100)
 * -------------------------------------------------------------------- */
int cat_set_mic_gain(int fd, int percent);               /* MGnn;  (0‑100) */
int cat_get_mic_gain(int fd, int *percent);              /* MG;    */
int cat_parse_mic_gain(const char *reply, int *percent);

/* --------------------------------------------------------------------
 * 17. Audio level (AL) – set/read (0‑100)
 * -------------------------------------------------------------------- */
int cat_set_audio_level(int fd, int percent);            /* ALnn;  (0‑100) */
int cat_get_audio_level(int fd, int *percent);           /* AL;    */
int cat_parse_audio_level(const char *reply, int *percent);

/* --------------------------------------------------------------------
 * 18. RIT (RI) – set/read (Hz, signed)
 * -------------------------------------------------------------------- */
int cat_set_rit(int fd, int hz);                         /* RI+nnn; or RI-nnn; */
int cat_get_rit(int fd, int *hz);                        /* RI;    */
int cat_parse_rit(const char *reply, int *hz);

/* --------------------------------------------------------------------
 * 19. XIT (XI) – set/read (Hz, signed)
 * -------------------------------------------------------------------- */
int cat_set_xit(int fd, int hz);                         /* XI+nnn; or XI-nnn; */
int cat_get_xit(int fd, int *hz);                        /* XI;    */
int cat_parse_xit(const char *reply, int *hz);

/* --------------------------------------------------------------------
 * 20. Filter width (FW) – set/read (kHz)
 * -------------------------------------------------------------------- */
int cat_set_filter_width(int fd, int khz);               /* FWnn;  (e.g. 12, 15, 20) */
int cat_get_filter_width(int fd, int *khz);              /* FW;    */
int cat_parse_filter_width(const char *reply, int *khz);

/* --------------------------------------------------------------------
 * 21. Data mode (DM) – set/read (0=off,1=on)
 * -------------------------------------------------------------------- */
int cat_set_data_mode(int fd, int on);                   /* DMn;   (0=off,1=on) */
int cat_get_data_mode(int fd, int *on);                  /* DM;    */
int cat_parse_data_mode(const char *reply, int *on);

/* --------------------------------------------------------------------
 * 22. CTCSS/DCS tone (CT) – set/read (tone index)
 * -------------------------------------------------------------------- */
int cat_set_ctcss_dcs(int fd, int index);                /* CTnn;  (0‑99) */
int cat_get_ctcss_dcs(int fd, int *index);               /* CT;    */
int cat_parse_ctcss_dcs(const char *reply, int *index);

/* --------------------------------------------------------------------
 * 23. Auto‑notch (AN) – set/read (0=off,1=on)
 * -------------------------------------------------------------------- */
int cat_set_auto_notch(int fd, int on);                  /* ANn;   (0=off,1=on) */
int cat_get_auto_notch(int fd, int *on);                 /* AN;    */
int cat_parse_auto_notch(const char *reply, int *on);

/* --------------------------------------------------------------------
 * 24. Noise blanker (NB) – set/read (0‑2)
 * -------------------------------------------------------------------- */
int cat_set_noise_blanker(int fd, int level);            /* NBn;   (0‑2) */
int cat_get_noise_blanker(int fd, int *level);           /* NB;    */
int cat_parse_noise_blanker(const char *reply, int *level);

/* --------------------------------------------------------------------
 * 25. AGC (AG) – set/read (0‑3)
 * -------------------------------------------------------------------- */
int cat_set_agc(int fd, int mode);                       /* AGn;   (0‑3) */
int cat_get_agc(int fd, int *mode);                      /* AG;    */
int cat_parse_agc(const char *reply, int *mode);

/* --------------------------------------------------------------------
 * 26. RF Attenuator (RA) – set/read (0=off,1=on)
 * -------------------------------------------------------------------- */
int cat_set_rf_attenuator(int fd, int on);               /* RAn;   (0=off,1=on) */
int cat_get_rf_attenuator(int fd, int *on);              /* RA;    */
int cat_parse_rf_attenuator(const char *reply, int *on);

/* --------------------------------------------------------------------
 * 27. Pre‑amp (PA) – set/read (0=off,1=on)
 * -------------------------------------------------------------------- */
int cat_set_preamp(int fd, int on);                      /* PAn;   (0=off,1=on) */
int cat_get_preamp(int fd, int *on);                     /* PA;    */
int cat_parse_preamp(const char *reply, int *on);

/* --------------------------------------------------------------------
 * 28. Squelch (SQ) – set/read (0‑100)
 * -------------------------------------------------------------------- */
int cat_set_squelch(int fd, int percent);                /* SQnn;  (0‑100) */
int cat_get_squelch(int fd, int *percent);               /* SQ;    */
int cat_parse_squelch(const char *reply, int *percent);

/* --------------------------------------------------------------------
 * 29. Keyer speed (KS) – set/read (WPM)
 * -------------------------------------------------------------------- */
int cat_set_keyer_speed(int fd, int wpm);                /* KSn;   (5‑60) */
int cat_get_keyer_speed(int fd, int *wpm);               /* KS;    */
int cat_parse_keyer_speed(const char *reply, int *wpm);

/* --------------------------------------------------------------------
 * 30. Keyer mode (KM) – set/read (0=straight,1=iambic A,2=iambic B)
 * -------------------------------------------------------------------- */
int cat_set_keyer_mode(int fd, int mode);                /* KMn;   (0‑2) */
int cat_get_keyer_mode(int fd, int *mode);               /* KM;    */
int cat_parse_keyer_mode(const char *reply, int *mode);

/* --------------------------------------------------------------------
 * 31. TX Delay (TD) – set/read (seconds, 0‑5)
 * -------------------------------------------------------------------- */
int cat_set_tx_delay(int fd, int seconds);               /* TDn;   (0‑5) */
int cat_get_tx_delay(int fd, int *seconds);              /* TD;    */
int cat_parse_tx_delay(const char *reply, int *seconds);

/* --------------------------------------------------------------------
 * 32. Memory channel name (MN) – set/read (up to 8 ASCII chars)
 * -------------------------------------------------------------------- */
int cat_set_memory_name(int fd, int mem_no, const char *name);   /* MNnn:name; */
int cat_get_memory_name(int fd, int mem_no, char *buf, size_t buflen); /* MNnn; */
int cat_parse_memory_name(const char *reply, char *out, size_t outlen);

/* --------------------------------------------------------------------
 * 33. Miscellaneous read‑only values (raw helpers)
 * -------------------------------------------------------------------- */
int cat_get_firmware_version_raw(int fd, cat_raw_reply_t *r);   /* VE; */
int cat_get_vox_gain_raw(int fd, cat_raw_reply_t *r);           /* VG; */
int cat_get_memory_raw(int fd, cat_raw_reply_t *r);            /* VM; */
int cat_get_vfo_select_raw(int fd, cat_raw_reply_t *r);        /* VS; */
int cat_get_vox_raw(int fd, cat_raw_reply_t *r);               /* VX; */
int cat_get_zero_in_raw(int fd, cat_raw_reply_t *r);           /* ZI; */

/* --------------------------------------------------------------------
 * 34. Generic helpers for any opcode not explicitly wrapped above
 * -------------------------------------------------------------------- */

/* Read‑only command (e.g. "FA;", "MD;", "VE;") */
int cat_read_command(int fd,
                     const char *opcode,   /* two‑letter opcode, without ';' */
                     char *reply,
                     size_t reply_len);

/* Write‑only command (e.g. "FA014250000;", "MD1;") */
int cat_write_command(int fd,
                      const char *opcode,   /* two‑letter opcode */
                      const char *payload);/* payload without trailing ';' */

/* --------------------------------------------------------------------
 * End of public API
 * -------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* FTX1_CAT_FULL_H */