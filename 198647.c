static int muscle_card_extract_key(sc_card_t *card, sc_cardctl_muscle_key_info_t *info)
{
	/* CURRENTLY DONT SUPPORT EXTRACTING PRIVATE KEYS... */
	switch(info->keyType) {
	case 1: /* RSA */
		return msc_extract_rsa_public_key(card,
			info->keyLocation,
			&info->modLength,
			&info->modValue,
			&info->expLength,
			&info->expValue);
	default:
		return SC_ERROR_NOT_SUPPORTED;
	}
}