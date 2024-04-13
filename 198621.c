epass2003_finish(sc_card_t *card)
{
	epass2003_exdata *exdata = (epass2003_exdata *)card->drv_data;

	if (exdata)
		free(exdata);
	return SC_SUCCESS;
}