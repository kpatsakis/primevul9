static int setcos_select_file(sc_card_t *card,
			      const sc_path_t *in_path, sc_file_t **file)
{
	int r;

	r = iso_ops->select_file(card, in_path, file);
	/* Certain FINeID cards for organisations return 6A88 instead of 6A82 for missing files */
	if (card->flags & _FINEID_BROKEN_SELECT_FLAG && r == SC_ERROR_DATA_OBJECT_NOT_FOUND)
		return SC_ERROR_FILE_NOT_FOUND;
	if (r)
		return r;
	if (file != NULL) {
		if (card->type == SC_CARD_TYPE_SETCOS_44 ||
		    card->type == SC_CARD_TYPE_SETCOS_NIDEL ||
		    SETCOS_IS_EID_APPLET(card))
			parse_sec_attr_44(*file, (*file)->sec_attr, (*file)->sec_attr_len);
		else
			parse_sec_attr(*file, (*file)->sec_attr, (*file)->sec_attr_len);
	}
	return 0;
}