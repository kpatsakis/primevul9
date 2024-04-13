static int cac_select_CCC(sc_card_t *card)
{
	return cac_select_file_by_type(card, &cac_CCC_Path, NULL, SC_CARD_TYPE_CAC_II);
}