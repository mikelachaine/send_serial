#include "ftx1_cat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Command Building Functions */

int cat_build_frequency_set(cat_command_t *cmd, vfo_select_t vfo, uint32_t freq_hz) {
    if (!cmd) return -1;
    
    if (vfo == VFO_MAIN) {
        strcpy(cmd->cmd, "FA");
    } else {
        strcpy(cmd->cmd, "FB");
    }
    
    snprintf(cmd->params, sizeof(cmd->params), "%09u", freq_hz);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_frequency_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    if (vfo == VFO_MAIN) {
        strcpy(cmd->cmd, "FA");
    } else {
        strcpy(cmd->cmd, "FB");
    }
    
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_mode_set(cat_command_t *cmd, vfo_select_t vfo, operating_mode_t mode) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "MD");
    snprintf(cmd->params, sizeof(cmd->params), "%d%02d", vfo, mode);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_mode_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "MD");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_af_gain_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t level) {
    if (!cmd || level > 255) return -1;
    
    strcpy(cmd->cmd, "AG");
    snprintf(cmd->params, sizeof(cmd->params), "%d%03d", vfo, level);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_af_gain_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "AG");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_rf_gain_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t level) {
    if (!cmd || level > 255) return -1;
    
    strcpy(cmd->cmd, "RG");
    snprintf(cmd->params, sizeof(cmd->params), "%d%03d", vfo, level);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_rf_gain_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "RG");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_squelch_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t level) {
    if (!cmd || level > 255) return -1;
    
    strcpy(cmd->cmd, "SQ");
    snprintf(cmd->params, sizeof(cmd->params), "%d%03d", vfo, level);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_squelch_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "SQ");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_power_set(cat_command_t *cmd, uint8_t watts) {
    if (!cmd || watts < 5 || watts > 100) return -1;
    
    strcpy(cmd->cmd, "PC");
    snprintf(cmd->params, sizeof(cmd->params), "%03d", watts);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_power_read(cat_command_t *cmd) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "PC");
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_agc_set(cat_command_t *cmd, vfo_select_t vfo, agc_type_t agc) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "GT");
    snprintf(cmd->params, sizeof(cmd->params), "%d%d", vfo, agc);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_agc_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "GT");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_band_up(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "BU");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_band_down(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "BD");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_band_select(cat_command_t *cmd, vfo_select_t vfo, band_select_t band) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "BS");
    snprintf(cmd->params, sizeof(cmd->params), "%d%02d", vfo, band);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_vfo_ab(cat_command_t *cmd) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "AB");
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_vfo_ba(cat_command_t *cmd) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "BA");
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_split_set(cat_command_t *cmd, bool enable) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "ST");
    snprintf(cmd->params, sizeof(cmd->params), "%d", enable ? 1 : 0);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_split_read(cat_command_t *cmd) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "ST");
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_ctcss_set(cat_command_t *cmd, vfo_select_t vfo, uint8_t type, uint8_t code) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "CN");
    snprintf(cmd->params, sizeof(cmd->params), "%d%d%02d", vfo, type, code);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_ctcss_read(cat_command_t *cmd, vfo_select_t vfo) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "CN");
    snprintf(cmd->params, sizeof(cmd->params), "%d", vfo);
    cmd->has_params = true;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_auto_info_set(cat_command_t *cmd, bool enable) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "AI");
    snprintf(cmd->params, sizeof(cmd->params), "%d", enable ? 1 : 0);
    cmd->has_params = true;
    cmd->type = CAT_CMD_SET;
    return 0;
}

int cat_build_firmware_version_read(cat_command_t *cmd) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "VE");
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_READ;
    return 0;
}

int cat_build_radio_info_read(cat_command_t *cmd) {
    if (!cmd) return -1;
    
    strcpy(cmd->cmd, "RI");
    cmd->params[0] = '\0';
    cmd->has_params = false;
    cmd->type = CAT_CMD_READ;
    return 0;
}

/* Command Parsing Functions */

int cat_parse_frequency_response(const char *response, frequency_info_t *freq_info) {
    if (!response || !freq_info) return -1;
    
    if (strncmp(response, "FA", 2) == 0) {
        freq_info->vfo = VFO_MAIN;
        freq_info->frequency = strtoul(response + 2, NULL, 10);
    } else if (strncmp(response, "FB", 2) == 0) {
        freq_info->vfo = VFO_SUB;
        freq_info->frequency = strtoul(response + 2, NULL, 10);
    } else {
        return -1;
    }
    
    return 0;
}

int cat_parse_mode_response(const char *response, mode_info_t *mode_info) {
    if (!response || !mode_info) return -1;
    
    if (strncmp(response, "MD", 2) != 0) return -1;
    
    char *ptr;
    int vfo_val = strtol(response + 2, &ptr, 10);
    if (vfo_val == 0 && ptr == response + 2) return -1;
    
    if (vfo_val == 0) {
        mode_info->vfo = VFO_MAIN;
    } else if (vfo_val == 1) {
        mode_info->vfo = VFO_SUB;
    } else {
        return -1;
    }
    
    int mode_val = strtol(ptr, NULL, 10);
    if (mode_val < 0 || mode_val > 99) return -1;
    
    mode_info->mode = (operating_mode_t)mode_val;
    return 0;
}

int cat_parse_af_gain_response(const char *response, gain_info_t *gain_info) {
    if (!response || !gain_info) return -1;
    
    if (strncmp(response, "AG", 2) != 0) return -1;
    
    char *ptr;
    int vfo_val = strtol(response + 2, &ptr, 10);
    if (vfo_val == 0 && ptr == response + 2) return -1;
    
    if (vfo_val == 0) {
        gain_info->vfo = VFO_MAIN;
    } else if (vfo_val == 1) {
        gain_info->vfo = VFO_SUB;
    } else {
        return -1;
    }
    
    int gain_val = strtol(ptr, NULL, 10);
    if (gain_val < 0 || gain_val > 255) return -1;
    
    gain_info->level = (uint8_t)gain_val;
    return 0;
}

int cat_parse_rf_gain_response(const char *response, gain_info_t *gain_info) {
    if (!response || !gain_info) return -1;
    
    if (strncmp(response, "RG", 2) != 0) return -1;
    
    char *ptr;
    int vfo_val = strtol(response + 2, &ptr, 10);
    if (vfo_val == 0 && ptr == response + 2) return -1;
    
    if (vfo_val == 0) {
        gain_info->vfo = VFO_MAIN;
    } else if (vfo_val == 1) {
        gain_info->vfo = VFO_SUB;
    } else {
        return -1;
    }
    
    int gain_val = strtol(ptr, NULL, 10);
    if (gain_val < 0 || gain_val > 255) return -1;
    
    gain_info->level = (uint8_t)gain_val;
    return 0;
}

int cat_parse_squelch_response(const char *response, squelch_info_t *squelch_info) {
    if (!response || !squelch_info) return -1;
    
    if (strncmp(response, "SQ", 2) != 0) return -1;
    
    char *ptr;
    int vfo_val = strtol(response + 2, &ptr, 10);
    if (vfo_val == 0 && ptr == response + 2) return -1;
    
    if (vfo_val == 0) {
        squelch_info->vfo = VFO_MAIN;
    } else if (vfo_val == 1) {
        squelch_info->vfo = VFO_SUB;
    } else {
        return -1;
    }
    
    int squelch_val = strtol(ptr, NULL, 10);
    if (squelch_val < 0 || squelch_val > 255) return -1;
    
    squelch_info->level = (uint8_t)squelch_val;
    return 0;
}

int cat_parse_power_response(const char *response, power_info_t *power_info) {
    if (!response || !power_info) return -1;
    
    if (strncmp(response, "PC", 2) != 0) return -1;
    
    unsigned long watts = strtoul(response + 2, NULL, 10);
    if (watts < 5 || watts > 100) return -1;
    
    power_info->watts = (uint8_t)watts;
    return 0;
}

int cat_parse_agc_response(const char *response, agc_info_t *agc_info) {
    if (!response || !agc_info) return -1;
    
    if (strncmp(response, "GT", 2) != 0) return -1;
    
    char *ptr;
    int vfo_val = strtol(response + 2, &ptr, 10);
    if (vfo_val == 0 && ptr == response + 2) return -1;
    
    if (vfo_val == 0) {
        agc_info->vfo = VFO_MAIN;
    } else if (vfo_val == 1) {
        agc_info->vfo = VFO_SUB;
    } else {
        return -1;
    }
    
    int agc_val = strtol(ptr, NULL, 10);
    if (agc_val < 0 || agc_val > 9) return -1;
    
    agc_info->agc = (agc_type_t)agc_val;
    return 0;
}

int cat_parse_split_response(const char *response, split_info_t *split_info) {
    if (!response || !split_info) return -1;
    
    if (strncmp(response, "ST", 2) != 0) return -1;
    
    int split_val = atoi(response + 2);
    if (split_val < 0 || split_val > 1) return -1;
    
    split_info->enabled = (bool)split_val;
    return 0;
}

int cat_parse_ctcss_response(const char *response, ctcss_info_t *ctcss_info) {
    if (!response || !ctcss_info) return -1;
    
    if (strncmp(response, "CN", 2) != 0) return -1;
    
    char *ptr;
    int vfo_val = strtol(response + 2, &ptr, 10);
    if (vfo_val == 0 && ptr == response + 2) return -1;
    
    if (vfo_val == 0) {
        ctcss_info->vfo = VFO_MAIN;
    } else if (vfo_val == 1) {
        ctcss_info->vfo = VFO_SUB;
    } else {
        return -1;
    }
    
    int type_val = strtol(ptr, &ptr, 10);
    if (type_val < 0 || type_val > 9) return -1;
    
    ctcss_info->type = (uint8_t)type_val;
    
    int code_val = strtol(ptr, NULL, 10);
    if (code_val < 0 || code_val > 99) return -1;
    
    ctcss_info->code = (uint8_t)code_val;
    return 0;
}

int cat_parse_firmware_version_response(const char *response, firmware_info_t *firmware_info) {
    if (!response || !firmware_info) return -1;
    
    if (strncmp(response, "VE", 2) != 0) return -1;
    
    strncpy(firmware_info->version, response + 2, sizeof(firmware_info->version) - 1);
    firmware_info->version[sizeof(firmware_info->version) - 1] = '\0';
    return 0;
}

int cat_parse_radio_info_response(const char *response, radio_info_t *radio_info) {
    if (!response || !radio_info) return -1;
    
    if (strncmp(response, "RI", 2) != 0) return -1;
    
    strncpy(radio_info->model, response + 2, sizeof(radio_info->model) - 1);
    radio_info->model[sizeof(radio_info->model) - 1] = '\0';
    return 0;
}
