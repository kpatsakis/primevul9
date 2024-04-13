static int setcos_construct_fci(sc_card_t *card, const sc_file_t *file, u8 *out, size_t *outlen)
{
	if (card->type == SC_CARD_TYPE_SETCOS_44 || 
	    card->type == SC_CARD_TYPE_SETCOS_NIDEL ||
	    SETCOS_IS_EID_APPLET(card))
		return setcos_construct_fci_44(card, file, out, outlen);
	else
		return iso_ops->construct_fci(card, file, out, outlen);
}