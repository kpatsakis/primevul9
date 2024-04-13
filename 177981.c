static int phar_zip_process_extra(php_stream *fp, phar_entry_info *entry, php_uint16 len) /* {{{ */
{
	union {
		phar_zip_extra_field_header header;
		phar_zip_unix3 unix3;
	} h;
	int read;

	do {
		if (sizeof(h.header) != php_stream_read(fp, (char *) &h.header, sizeof(h.header))) {
			return FAILURE;
		}

		if (h.header.tag[0] != 'n' || h.header.tag[1] != 'u') {
			/* skip to next header */
			php_stream_seek(fp, PHAR_GET_16(h.header.size), SEEK_CUR);
			len -= PHAR_GET_16(h.header.size) + 4;
			continue;
		}

		/* unix3 header found */
		read = php_stream_read(fp, (char *) &(h.unix3.crc32), sizeof(h.unix3) - sizeof(h.header));
		len -= read + 4;

		if (sizeof(h.unix3) - sizeof(h.header) != read) {
			return FAILURE;
		}

		if (PHAR_GET_16(h.unix3.size) > sizeof(h.unix3) - 4) {
			/* skip symlink filename - we may add this support in later */
			php_stream_seek(fp, PHAR_GET_16(h.unix3.size) - sizeof(h.unix3.size), SEEK_CUR);
		}

		/* set permissions */
		entry->flags &= PHAR_ENT_COMPRESSION_MASK;

		if (entry->is_dir) {
			entry->flags |= PHAR_GET_16(h.unix3.perms) & PHAR_ENT_PERM_MASK;
		} else {
			entry->flags |= PHAR_GET_16(h.unix3.perms) & PHAR_ENT_PERM_MASK;
		}

	} while (len);

	return SUCCESS;
}