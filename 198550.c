static int muscle_card_generate_key(sc_card_t *card, sc_cardctl_muscle_gen_key_info_t *info)
{
	return msc_generate_keypair(card,
		info->privateKeyLocation,
		info->publicKeyLocation,
		info->keyType,
		info->keySize,
		0);
}