static int cac_process_CCC(sc_card_t *card, cac_private_data_t *priv)
{
	u8 *tl = NULL, *val = NULL;
	size_t tl_len, val_len;
	int r;


	r = cac_read_file(card, CAC_FILE_TAG, &tl, &tl_len);
	if (r < 0)
		goto done;

	r = cac_read_file(card, CAC_FILE_VALUE, &val, &val_len);
	if (r < 0)
		goto done;

	r = cac_parse_CCC(card, priv, tl, tl_len, val, val_len);
done:
	if (tl)
		free(tl);
	if (val)
		free(val);
	return r;
}