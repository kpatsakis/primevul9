static int setcos_init(sc_card_t *card)
{
	card->name = "SetCOS";

	/* Handle unknown or forced cards */
	if (card->type < 0) {
		card->type = SC_CARD_TYPE_SETCOS_GENERIC;
	}

	switch (card->type) {
	case SC_CARD_TYPE_SETCOS_FINEID:
	case SC_CARD_TYPE_SETCOS_FINEID_V2_2048:
	case SC_CARD_TYPE_SETCOS_NIDEL:
		card->cla = 0x00;
		select_pkcs15_app(card);
		if (card->flags & SC_CARD_FLAG_RNG)
			card->caps |= SC_CARD_CAP_RNG;
		break;
	case SC_CARD_TYPE_SETCOS_44:
	case SC_CARD_TYPE_SETCOS_EID_V2_0:
	case SC_CARD_TYPE_SETCOS_EID_V2_1:
		card->cla = 0x00;
		card->caps |= SC_CARD_CAP_USE_FCI_AC;
		card->caps |= SC_CARD_CAP_RNG;
		card->caps |= SC_CARD_CAP_APDU_EXT;
		break;
	default:
		/* XXX: Get SetCOS version */
		card->cla = 0x80;	/* SetCOS 4.3.x */
		/* State that we have an RNG */
		card->caps |= SC_CARD_CAP_RNG;
		break;
	}

	switch (card->type) {
	case SC_CARD_TYPE_SETCOS_PKI:
	case SC_CARD_TYPE_SETCOS_FINEID_V2_2048:
		{
			unsigned long flags;

			flags = SC_ALGORITHM_RSA_RAW | SC_ALGORITHM_RSA_PAD_PKCS1;
			flags |= SC_ALGORITHM_RSA_HASH_NONE | SC_ALGORITHM_RSA_HASH_SHA1;

			_sc_card_add_rsa_alg(card, 1024, flags, 0);
			_sc_card_add_rsa_alg(card, 2048, flags, 0);
		}
		break;
	case SC_CARD_TYPE_SETCOS_44:
	case SC_CARD_TYPE_SETCOS_NIDEL:
	case SC_CARD_TYPE_SETCOS_EID_V2_0:
	case SC_CARD_TYPE_SETCOS_EID_V2_1:
		{
			unsigned long flags;

			flags = SC_ALGORITHM_RSA_RAW | SC_ALGORITHM_RSA_PAD_PKCS1;
			flags |= SC_ALGORITHM_RSA_HASH_NONE | SC_ALGORITHM_RSA_HASH_SHA1;
			flags |= SC_ALGORITHM_ONBOARD_KEY_GEN;

			_sc_card_add_rsa_alg(card, 512, flags, 0);
			_sc_card_add_rsa_alg(card, 768, flags, 0);
			_sc_card_add_rsa_alg(card, 1024, flags, 0);
			_sc_card_add_rsa_alg(card, 2048, flags, 0);
		}
		break;
	}
	return 0;
}