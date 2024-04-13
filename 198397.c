static int cac_select_file(sc_card_t *card, const sc_path_t *in_path, sc_file_t **file_out)
{
	return cac_select_file_by_type(card, in_path, file_out, card->type);
}