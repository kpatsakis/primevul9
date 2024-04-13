static int setcos_create_file(sc_card_t *card, sc_file_t *file)
{
	if (card->type == SC_CARD_TYPE_SETCOS_44 || SETCOS_IS_EID_APPLET(card))
		return setcos_create_file_44(card, file);

	if (file->prop_attr_len == 0)
		sc_file_set_prop_attr(file, (const u8 *) "\x03\x00\x00", 3);
	if (file->sec_attr_len == 0) {
		int idx[6], i;
		u8 buf[6];

		if (file->type == SC_FILE_TYPE_DF) {
			const int df_idx[6] = {
				SC_AC_OP_SELECT, SC_AC_OP_LOCK, SC_AC_OP_DELETE,
				SC_AC_OP_CREATE, SC_AC_OP_REHABILITATE,
				SC_AC_OP_INVALIDATE
			};
			for (i = 0; i < 6; i++)
				idx[i] = df_idx[i];
		} else {
			const int ef_idx[6] = {
				SC_AC_OP_READ, SC_AC_OP_UPDATE, SC_AC_OP_WRITE,
				SC_AC_OP_ERASE, SC_AC_OP_REHABILITATE,
				SC_AC_OP_INVALIDATE
			};
			for (i = 0; i < 6; i++)
				idx[i] = ef_idx[i];
		}
		for (i = 0; i < 6; i++) {
			const struct sc_acl_entry *entry;
			entry = sc_file_get_acl_entry(file, idx[i]);
			buf[i] = acl_to_byte(entry);
		}

		sc_file_set_sec_attr(file, buf, 6);
	}

	return iso_ops->create_file(card, file);
}