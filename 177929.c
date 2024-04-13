static void cmd_read(IDEState *s, uint8_t* buf)
{
    int nb_sectors, lba;

    if (buf[0] == GPCMD_READ_10) {
        nb_sectors = ube16_to_cpu(buf + 7);
    } else {
        nb_sectors = ube32_to_cpu(buf + 6);
    }

    lba = ube32_to_cpu(buf + 2);
    if (nb_sectors == 0) {
        ide_atapi_cmd_ok(s);
        return;
    }

    ide_atapi_cmd_read(s, lba, nb_sectors, 2048);
}