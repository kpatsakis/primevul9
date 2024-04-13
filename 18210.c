static ut8 *get_section_bytes(RzBinFile *binfile, const char *sect_name, size_t *len) {
	rz_return_val_if_fail(binfile && sect_name && len, NULL);
	RzBinSection *section = getsection(binfile, sect_name);
	if (!section) {
		return NULL;
	}
	if (section->size > binfile->size) {
		return NULL;
	}
	*len = section->size;
	ut8 *buf = calloc(1, *len);
	rz_buf_read_at(binfile->buf, section->paddr, buf, *len);
	return buf;
}