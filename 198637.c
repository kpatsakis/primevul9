static int cac_find_first_pki_applet(sc_card_t *card, int *index_out)
{
	int r, i;
	for (i = 0; i < MAX_CAC_SLOTS; i++) {
		r = cac_select_pki_applet(card, i);
		if (r == SC_SUCCESS) {
			/* Try to read first two bytes of the buffer to
			 * make sure it is not just malfunctioning card
			 */
			u8 params[2] = {CAC_FILE_TAG, 2};
			u8 data[2], *out_ptr = data;
			size_t len = 2;
			r = cac_apdu_io(card, CAC_INS_READ_FILE, 0, 0,
			    &params[0], sizeof(params), &out_ptr, &len);
			if (r != 2)
				continue;

			*index_out = i;
			return SC_SUCCESS;
		}
	}
	return SC_ERROR_OBJECT_NOT_FOUND;
}