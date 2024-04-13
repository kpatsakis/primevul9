
static unsigned int cli_md5sect(fmap_t *map, struct cli_exe_section *s, unsigned char *digest) {
    void *hashme;
    cli_md5_ctx md5;

    if (s->rsz > CLI_MAX_ALLOCATION) {
	cli_dbgmsg("cli_md5sect: skipping md5 calculation for too big section\n");
	return 0;
    }

    if(!s->rsz) return 0;
    if(!(hashme=fmap_need_off_once(map, s->raw, s->rsz))) {
	cli_dbgmsg("cli_md5sect: unable to read section data\n");
	return 0;
    }

    cli_md5_init(&md5);
    cli_md5_update(&md5, hashme, s->rsz);
    cli_md5_final(digest, &md5);
    cli_dbgmsg("MDB: %u:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
        s->rsz, digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6],
        digest[7], digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14],
        digest[15]);
    return 1;