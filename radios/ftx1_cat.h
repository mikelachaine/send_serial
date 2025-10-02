#ifndef FTX1_CAT_H
#define FTX1_CAT_H

#include <stdint.h>
#include <stdbool.h>

/* CAT Command Types */
typedef enum {
    CAT_CMD_SET,
    CAT_CMD_READ,
    CAT_CMD_ANSWER
} cat_cmd_type_t;

/* VFO Selection */
typedef enum {
    VFO_MAIN = 0,
    VFO_SUB = 1
} vfo_select_t;

/* Operating Modes */
typedef enum {
    MODE_LSB = 1,
    MODE_USB = 2,
    MODE_CW = 3,
    MODE_FM = 4,
    MODE_AM = 5,
    MODE_RTTY_LSB = 6,
    MODE_CW_R = 7,
    MODE_DATA_LSB = 8,
    MODE_RTTY_USB = 9,
    MODE_DATA_FM = 10,
    MODE_FM_N = 11,
    MODE_DATA_USB = 12,
    MODE_AM_N = 13,
    MODE_C4FM = 14
} operating_mode_t;

/* Band Selection */
typedef enum {
    BAND_1_8MHZ = 0,
    BAND_3_5MHZ = 1,
    BAND_5MHZ = 2,
    BAND_7MHZ = 3,
    BAND_10MHZ = 4,
    BAND_14MHZ = 5,
    BAND_18MHZ = 6,
    BAND_21MHZ = 7,
    BAND_24_5MHZ = 8,
    BAND_28MHZ = 9,
    BAND_50MHZ = 10,
    BAND_70MHZ_GEN = 11,
    BAND_AIR = 12,
    BAND_144MHZ = 13,
    BAND_430MHZ = 14
} band_select_t;

/* AGC Types */
typedef enum {
    AGC_AUTO = 0,
    AGC_FAST = 1,
    AGC_MID = 2,
    AGC_SLOW = 3,
    AGC_OFF = 4
} agc_type_t;

/* CAT Command Structure */
typedef struct {
    char cmd[3];        /* 2-character command + null terminator */
    char params[32];    /* Parameter string */
    bool has_params;    /* Whether command has parameters */
    cat_cmd_type_t type; /* Command type */
} cat_command_t;

/* Frequency Structure */
typedef struct {
    uint32_t frequency; /* Frequency in Hz */
    vfo_select_t vfo;   /* VFO selection */
} frequency_info_t;

/* Mode Structure */
typedef struct {
    operating_mode_t mode;
    vfo_select_t vfo;
} mode_info_t;

/* AGC Structure */
typedef struct {
    agc_type_t type;
    vfo_select_t vfo;
} agc_info_t;

/* AF Gain Structure */
typedef struct {
    uint8_t level;      /* 0-255 */
    vfo_select_t vfo;
} af_gain_info_t;

/* RF Gain Structure */
typedef struct {
    uint8_t level;      /* 0-255 */
    vfo_select_t vfo;
} rf_gain_info_t;

/* Squelch Structure */
typedef struct {
    uint8_t level;      /* 0-255 */
    vfo_select_t vfo;
} squelch_info_t;

/* Power Control Structure */
typedef struct {
    uint8_t level;      /* 5-100 watts */
} power_control_t;

/* CTCSS/DCS Structure */
typedef struct {
    vfo_select_t vfo;
    uint8_t type;       /* 0: CTCSS, 1: DCS */
    uint8_t code;       /* Tone/DCS code number */
} ctcss_dcs_info_t;

/* Function Prototypes */

/* Command Building Functions */
int cat_build_frequency_set(cat_command_t *cmd, vfo_select_t vfo, uint32_t freq_hz);
int cat_build_frequency_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_mode_set(cat_command_t *cmd, vfo_select_t vfo, operating_mode_t mode);
int cat_build_mode_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_af_gain_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t level);
int cat_build_af_gain_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_rf_gain_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t level);
int cat_build_rf_gain_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_squelch_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t level);
int cat_build_squelch_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_power_set(cat_command_t *cmd, uint8_t watts);
int cat_build_power_read(cat_command_t *cmd);
int cat_build_agc_set(cat_command_t *cmd, vfo_select_t vfo, agc_type_t agc);
int cat_build_agc_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_band_up(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_band_down(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_band_select(cat_command_t *cmd, vfo_select_t vfo, band_select_t band);
int cat_build_vfo_ab(cat_command_t *cmd);
int cat_build_vfo_ba(cat_command_t *cmd);
int cat_build_split_set(cat_command_t *cmd, bool enable);
int cat_build_split_read(cat_command_t *cmd);
int cat_build_ctcss_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t type, uint8_t code);
int cat_build_ctcss_read(cat_command_t *cmd, vfo_select_t vfo);
int cat_build_auto_info_set(cat_command_t *cmd, bool enable);
int cat_build_firmware_version_read(cat_command_t *cmd);
int cat_build_radio_info_read(cat_command_t *cmd);

/* Command Parsing Functions */
int cat_parse_frequency_response(const char *response, frequency_info_t *freq_info);
int cat_parse_mode_response(const char *response, mode_info_t *mode_info);
int cat_parse_af_gain_response(const char *response, af_gain_info_t *af_info);
int cat_parse_rf_gain_response(const char *response, rf_gain_info_t *rf_info);
int cat_parse_squelch_response(const char *response, squelch_info_t *sq_info);
int cat_parse_power_response(const char *response, power_control_t *power_info);
int cat_parse_agc_response(const char *response, agc_info_t *agc_info);
int cat_parse_split_response(const char *response, bool *split_enabled);
int cat_parse_ctcss_response(const char *response, ctcss_dcs_info_t *ctcss_info);

/* Utility Functions */
const char* cat_command_to_string(const cat_command_t *cmd);
int cat_validate_frequency(uint32_t freq_hz, band_select_t *suggested_band);
const char* cat_mode_to_string(operating_mode_t mode);
const char* cat_band_to_string(band_select_t band);
const char* cat_agc_to_string(agc_type_t agc);
bool cat_is_valid_response(const char *response);
int cat_get_response_length(const char *cmd);

#endif /* FTX1_CAT_H */