static void cmd_read_disc_information(IDEState *s, uint8_t* buf)
{
    uint8_t type = buf[1] & 7;
    uint32_t max_len = ube16_to_cpu(buf + 7);

    /* Types 1/2 are only defined for Blu-Ray.  */
    if (type != 0) {
        ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                            ASC_INV_FIELD_IN_CMD_PACKET);
        return;
    }

    memset(buf, 0, 34);
    buf[1] = 32;
    buf[2] = 0xe; /* last session complete, disc finalized */
    buf[3] = 1;   /* first track on disc */
    buf[4] = 1;   /* # of sessions */
    buf[5] = 1;   /* first track of last session */
    buf[6] = 1;   /* last track of last session */
    buf[7] = 0x20; /* unrestricted use */
    buf[8] = 0x00; /* CD-ROM or DVD-ROM */
    /* 9-10-11: most significant byte corresponding bytes 4-5-6 */
    /* 12-23: not meaningful for CD-ROM or DVD-ROM */
    /* 24-31: disc bar code */
    /* 32: disc application code */
    /* 33: number of OPC tables */

    ide_atapi_cmd_reply(s, 34, max_len);
}