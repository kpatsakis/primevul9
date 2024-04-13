static int create_pin_file(const sc_path_t *inpath, int chv, const char *key_id)
{
	char prompt[40], *pin, *puk;
	char buf[30], *p = buf;
	sc_path_t file_id, path;
	sc_file_t *file;
	size_t len;
	int r;

	file_id = *inpath;
	if (file_id.len < 2)
		return -1;
	if (chv == 1)
		sc_format_path("I0000", &file_id);
	else if (chv == 2)
		sc_format_path("I0100", &file_id);
	else
		return -1;
	r = sc_select_file(card, inpath, NULL);
	if (r)
		return -1;
	r = sc_select_file(card, &file_id, NULL);
	if (r == 0)
		return 0;

	sprintf(prompt, "Please enter CHV%d%s: ", chv, key_id);
	pin = getpin(prompt);
	if (pin == NULL)
		return -1;

	sprintf(prompt, "Please enter PUK for CHV%d%s: ", chv, key_id);
	puk = getpin(prompt);
	if (puk == NULL) {
		free(pin);
		return -1;
	}

	memset(p, 0xFF, 3);
	p += 3;
	memcpy(p, pin, 8);
	p += 8;
	*p++ = opt_pin_attempts;
	*p++ = opt_pin_attempts;
	memcpy(p, puk, 8);
	p += 8;
	*p++ = opt_puk_attempts;
	*p++ = opt_puk_attempts;
	len = p - buf;

	free(pin);
	free(puk);

	file = sc_file_new();
	file->type = SC_FILE_TYPE_WORKING_EF;
	file->ef_structure = SC_FILE_EF_TRANSPARENT;
	sc_file_add_acl_entry(file, SC_AC_OP_READ, SC_AC_NEVER, SC_AC_KEY_REF_NONE);
	if (inpath->len == 2 && inpath->value[0] == 0x3F &&
	    inpath->value[1] == 0x00)
		sc_file_add_acl_entry(file, SC_AC_OP_UPDATE, SC_AC_AUT, 1);
	else
		sc_file_add_acl_entry(file, SC_AC_OP_UPDATE, SC_AC_CHV, 2);

	sc_file_add_acl_entry(file, SC_AC_OP_INVALIDATE, SC_AC_AUT, 1);
	sc_file_add_acl_entry(file, SC_AC_OP_REHABILITATE, SC_AC_AUT, 1);
	file->size = len;
	file->id = (file_id.value[0] << 8) | file_id.value[1];
	r = sc_create_file(card, file);
	sc_file_free(file);
	if (r) {
		fprintf(stderr, "PIN file creation failed: %s\n", sc_strerror(r));
		return r;
	}
	path = *inpath;
	sc_append_path(&path, &file_id);
	r = sc_select_file(card, &path, NULL);
	if (r) {
		fprintf(stderr, "Unable to select created PIN file: %s\n", sc_strerror(r));
		return r;
	}
	r = sc_update_binary(card, 0, (const u8 *) buf, len, 0);
	if (r < 0) {
		fprintf(stderr, "Unable to update created PIN file: %s\n", sc_strerror(r));
		return r;
	}

	return 0;
}