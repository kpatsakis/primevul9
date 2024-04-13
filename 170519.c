int phar_is_tar(char *buf, char *fname) /* {{{ */
{
	tar_header *header = (tar_header *) buf;
	uint32_t checksum = phar_tar_number(header->checksum, sizeof(header->checksum));
	uint32_t ret;
	char save[sizeof(header->checksum)], *bname;

	/* assume that the first filename in a tar won't begin with <?php */
	if (!strncmp(buf, "<?php", sizeof("<?php")-1)) {
		return 0;
	}

	memcpy(save, header->checksum, sizeof(header->checksum));
	memset(header->checksum, ' ', sizeof(header->checksum));
	ret = (checksum == phar_tar_checksum(buf, 512));
	memcpy(header->checksum, save, sizeof(header->checksum));
	if ((bname = strrchr(fname, PHP_DIR_SEPARATOR))) {
		fname = bname;
	}
	if (!ret && (bname = strstr(fname, ".tar")) && (bname[4] == '\0' || bname[4] == '.')) {
		/* probably a corrupted tar - so we will pretend it is one */
		return 1;
	}
	return ret;
}