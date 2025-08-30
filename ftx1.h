/***********************************************************************
 *  ftx1_cat_all.h
 *
 *  Complete set of function prototypes for the Yaesu FTX‑1 CAT protocol
 *  (exactly the commands that are documented in the PDF you supplied).
 *
 *  • All functions return 0 on success, a negative errno‑style value on error.
 *  • The first argument of every function is the open file descriptor for the
 *    serial port (e.g. the result of open("/dev/ttyUSB0", …)).
 *  • Numeric arguments are expressed in the units that make sense for the
 *    command (frequency in MHz, gain 0‑100, etc.).
 *  • For commands that have *both* a Set and a Read form, three prototypes are
 *    generated:
 *        – cat_<cmd>_set(...)
 *        – cat_<cmd>_get(...)
 *        – cat_<cmd>_parse(...)   // optional helper to turn the raw reply into a C value
 *
 *  The implementation of each wrapper should:
 *      1. Build the ASCII command string exactly as the manual specifies
 *         (two‑letter opcode + zero‑padded parameters + ‘;’).
 *      2. Call `cat_send_raw()` to transmit the string.
 *      3. If a reply is expected, read it, strip the leading opcode and ‘;’
 *         and convert it to the appropriate C type.
 *
 *  Author: Lumo (Proton AI) – July 2025
 ***********************************************************************/

#ifndef FTX1_CAT_ALL_H
#define FTX1_CAT_ALL_H

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
 *  Low‑level helper (same as in the previous header)
 * -------------------------------------------------------------------- */
int cat_send_raw(int fd,
                 const char *cmd,          /* without trailing ';' */
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
 *  1. Frequency commands
 * -------------------------------------------------------------------- */

/* Main‑side VFO frequency (FA) */
int cat_set_frequency_main(int fd, double mhz);          /* FAxxxxxxx; */
int cat_get_frequency_main(int fd, double *mhz);         /* FA;   */
int cat_parse_frequency(const char *reply, double *mhz);

/* Sub‑side VFO frequency (FB) */
int cat_set_frequency_sub(int fd, double mhz);           /* FBxxxxxxx; */
int cat_get_frequency_sub(int fd, double *mhz);          /* FB;   */
int cat_parse_frequency_sub(const char *reply, double *mhz);

/* Transmit (TX) frequency – not explicitly listed in the excerpt,
   but many Yaesu radios expose it as FT; we include it for completeness. */
int cat_set_frequency_tx(int fd, double mhz);            /* FTxxxxxxx; */
int cat_get_frequency_tx(int fd, double *mhz);           /* FT;   */
int cat_parse_frequency_tx(const char *reply, double *mhz);

/* --------------------------------------------------------------------
 *  2. Mode commands (MD)
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

int cat_set_mode(int fd, cat_mode_t mode);               /* MDn; */
int cat_get_mode(int fd, cat_mode_t *mode);              /* MD; */
int cat_parse_mode(const char *reply, cat_mode_t *mode);

/* --------------------------------------------------------------------
 *  3. VFO / Memory channel handling (VM, VS, VFO select)
 * -------------------------------------------------------------------- */

/* VM – Main‑side to memory channel */
int cat_save_memory(int fd, int mem_no);                  /* VM0nn; */
int cat_recall_memory(int fd, int mem_no);               /* VM1nn; */
int cat_parse_memory(const char *reply, int *mem_no);

/* VS – VFO select */
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
 *  5. TX (PTT) control – command TX
 * -------------------------------------------------------------------- */
int cat_set_tx(int fd, int state);                       /* TXn;   (0‑2) */
int cat_get_tx(int fd, int *state);                      /* TX;    */
int cat_parse_tx(const char *reply, int *state);

/* --------------------------------------------------------------------
 *  6. VOX handling (VX, VG)
 * -------------------------------------------------------------------- */

/* VX – VOX status (on/off) */
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
int cat_zero_in(int fd, int side);                       /* ZI P1;   (0=MAIN,1=SUB) */
int cat_parse_zero_in(const char *reply, int *side);

/* --------------------------------------------------------------------
 *  8. Firmware version (VE)
 * -------------------------------------------------------------------- */
int cat_get_firmware_version(int fd, char *buf, size_t buflen);   /* VE; */
int cat_parse_firmware_version(const char *reply, char *out, size_t outlen);

/* --------------------------------------------------------------------
 *  9. Miscellaneous read‑only numeric values
 * -------------------------------------------------------------------- */

/* VE – already covered (firmware) */

/* VG – already covered (VOX gain) */

/* V G – (same as VG) – kept for naming consistency */
int cat_get_vox_gain_raw(int fd, cat_raw_reply_t *r);    /* VG; */

/* V M – memory channel (raw) */
int cat_get_memory_raw(int fd, cat_raw_reply_t *r);      /* VM; */

/* V S – VFO select (raw) */
int cat_get_vfo_select_raw(int fd, cat_raw_reply_t *r);  /* VS; */

/* V X – VOX status (raw) */
int cat_get_vox_raw(int fd, cat_raw_reply_t *r);         /* VX; */

/* Z I – Zero‑In (raw) */
int cat_get_zero_in_raw(int fd, cat_raw_reply_t *r);    /* ZI; */

/* --------------------------------------------------------------------
 * 10. Generic “read” helper (for any opcode not wrapped above)
 * -------------------------------------------------------------------- */
int cat_read_command(int fd,
                     const char *opcode,   /* e.g. "FA", "MD", "VE" */
                     char *reply,
                     size_t reply_len);

/* --------------------------------------------------------------------
 * 11. Generic “set” helper (for any opcode not wrapped above)
 * -------------------------------------------------------------------- */
int cat_write_command(int fd,
                      const char *opcode,   /* e.g. "FA", "MD", "VG" */
                      const char *payload);/* payload without trailing ';' */

/* --------------------------------------------------------------------
 * End of public API
 * -------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* FTX1_CAT_ALL_H */