static void cmd_mode_sense(IDEState *s, uint8_t *buf)
{
    int action, code;
    int max_len;

    max_len = ube16_to_cpu(buf + 7);
    action = buf[2] >> 6;
    code = buf[2] & 0x3f;

    switch(action) {
    case 0: /* current values */
        switch(code) {
        case MODE_PAGE_R_W_ERROR: /* error recovery */
            cpu_to_ube16(&buf[0], 16 - 2);
            buf[2] = 0x70;
            buf[3] = 0;
            buf[4] = 0;
            buf[5] = 0;
            buf[6] = 0;
            buf[7] = 0;

            buf[8] = MODE_PAGE_R_W_ERROR;
            buf[9] = 16 - 10;
            buf[10] = 0x00;
            buf[11] = 0x05;
            buf[12] = 0x00;
            buf[13] = 0x00;
            buf[14] = 0x00;
            buf[15] = 0x00;
            ide_atapi_cmd_reply(s, 16, max_len);
            break;
        case MODE_PAGE_AUDIO_CTL:
            cpu_to_ube16(&buf[0], 24 - 2);
            buf[2] = 0x70;
            buf[3] = 0;
            buf[4] = 0;
            buf[5] = 0;
            buf[6] = 0;
            buf[7] = 0;

            buf[8] = MODE_PAGE_AUDIO_CTL;
            buf[9] = 24 - 10;
            /* Fill with CDROM audio volume */
            buf[17] = 0;
            buf[19] = 0;
            buf[21] = 0;
            buf[23] = 0;

            ide_atapi_cmd_reply(s, 24, max_len);
            break;
        case MODE_PAGE_CAPABILITIES:
            cpu_to_ube16(&buf[0], 30 - 2);
            buf[2] = 0x70;
            buf[3] = 0;
            buf[4] = 0;
            buf[5] = 0;
            buf[6] = 0;
            buf[7] = 0;

            buf[8] = MODE_PAGE_CAPABILITIES;
            buf[9] = 30 - 10;
            buf[10] = 0x3b; /* read CDR/CDRW/DVDROM/DVDR/DVDRAM */
            buf[11] = 0x00;

            /* Claim PLAY_AUDIO capability (0x01) since some Linux
               code checks for this to automount media. */
            buf[12] = 0x71;
            buf[13] = 3 << 5;
            buf[14] = (1 << 0) | (1 << 3) | (1 << 5);
            if (s->tray_locked) {
                buf[14] |= 1 << 1;
            }
            buf[15] = 0x00; /* No volume & mute control, no changer */
            cpu_to_ube16(&buf[16], 704); /* 4x read speed */
            buf[18] = 0; /* Two volume levels */
            buf[19] = 2;
            cpu_to_ube16(&buf[20], 512); /* 512k buffer */
            cpu_to_ube16(&buf[22], 704); /* 4x read speed current */
            buf[24] = 0;
            buf[25] = 0;
            buf[26] = 0;
            buf[27] = 0;
            buf[28] = 0;
            buf[29] = 0;
            ide_atapi_cmd_reply(s, 30, max_len);
            break;
        default:
            goto error_cmd;
        }
        break;
    case 1: /* changeable values */
        goto error_cmd;
    case 2: /* default values */
        goto error_cmd;
    default:
    case 3: /* saved values */
        ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                            ASC_SAVING_PARAMETERS_NOT_SUPPORTED);
        break;
    }
    return;

error_cmd:
    ide_atapi_cmd_error(s, ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET);
}