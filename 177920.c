static void cmd_seek(IDEState *s, uint8_t* buf)
{
    unsigned int lba;
    uint64_t total_sectors = s->nb_sectors >> 2;

    lba = ube32_to_cpu(buf + 2);
    if (lba >= total_sectors) {
        ide_atapi_cmd_error(s, ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR);
        return;
    }

    ide_atapi_cmd_ok(s);
}