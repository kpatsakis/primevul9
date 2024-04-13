static int tcos_init(sc_card_t *card)
{
        unsigned long flags;

	tcos_data *data = malloc(sizeof(tcos_data));
	if (!data) return SC_ERROR_OUT_OF_MEMORY;

	card->name = "TCOS";
	card->drv_data = (void *)data;
	card->cla = 0x00;

        flags = SC_ALGORITHM_RSA_RAW;
        flags |= SC_ALGORITHM_RSA_PAD_PKCS1;
        flags |= SC_ALGORITHM_RSA_HASH_NONE;

        _sc_card_add_rsa_alg(card, 512, flags, 0);
        _sc_card_add_rsa_alg(card, 768, flags, 0);
        _sc_card_add_rsa_alg(card, 1024, flags, 0);

	if (card->type == SC_CARD_TYPE_TCOS_V3) {
		card->caps |= SC_CARD_CAP_APDU_EXT;
		_sc_card_add_rsa_alg(card, 1280, flags, 0);
		_sc_card_add_rsa_alg(card, 1536, flags, 0);
		_sc_card_add_rsa_alg(card, 1792, flags, 0);
        	_sc_card_add_rsa_alg(card, 2048, flags, 0);
	}

	return 0;
}