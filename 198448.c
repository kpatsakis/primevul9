static int muscle_card_import_key(sc_card_t *card, sc_cardctl_muscle_key_info_t *info)
{
	/* CURRENTLY DONT SUPPORT EXTRACTING PRIVATE KEYS... */
	switch(info->keyType) {
	case 0x02: /* RSA_PRIVATE */
	case 0x03: /* RSA_PRIVATE_CRT */
		return msc_import_key(card,
			info->keyLocation,
			info);
	default:
		return SC_ERROR_NOT_SUPPORTED;
	}
}