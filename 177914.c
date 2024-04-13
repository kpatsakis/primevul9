static void cmd_read_dvd_structure(IDEState *s, uint8_t* buf)
{
    int max_len;
    int media = buf[1];
    int format = buf[7];
    int ret;

    max_len = ube16_to_cpu(buf + 8);

    if (format < 0xff) {
        if (media_is_cd(s)) {
            ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                                ASC_INCOMPATIBLE_FORMAT);
            return;
        } else if (!media_present(s)) {
            ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                                ASC_INV_FIELD_IN_CMD_PACKET);
            return;
        }
    }

    memset(buf, 0, max_len > IDE_DMA_BUF_SECTORS * 512 + 4 ?
           IDE_DMA_BUF_SECTORS * 512 + 4 : max_len);

    switch (format) {
        case 0x00 ... 0x7f:
        case 0xff:
            if (media == 0) {
                ret = ide_dvd_read_structure(s, format, buf, buf);

                if (ret < 0) {
                    ide_atapi_cmd_error(s, ILLEGAL_REQUEST, -ret);
                } else {
                    ide_atapi_cmd_reply(s, ret, max_len);
                }

                break;
            }
            /* TODO: BD support, fall through for now */

        /* Generic disk structures */
        case 0x80: /* TODO: AACS volume identifier */
        case 0x81: /* TODO: AACS media serial number */
        case 0x82: /* TODO: AACS media identifier */
        case 0x83: /* TODO: AACS media key block */
        case 0x90: /* TODO: List of recognized format layers */
        case 0xc0: /* TODO: Write protection status */
        default:
            ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                                ASC_INV_FIELD_IN_CMD_PACKET);
            break;
    }
}