static int coolkey_init(sc_card_t *card)
{
	int r;
	unsigned long flags;
	unsigned long ext_flags;
	coolkey_private_data_t * priv;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	r = coolkey_initialize(card);
	if (r < 0) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_CARD);
	}

	card->type = SC_CARD_TYPE_COOLKEY_GENERIC;

	/* set Token Major/minor version */
	flags = SC_ALGORITHM_RSA_RAW;

	_sc_card_add_rsa_alg(card, 1024, flags, 0); /* mandatory */
	_sc_card_add_rsa_alg(card, 2048, flags, 0); /* optional */
	_sc_card_add_rsa_alg(card, 3072, flags, 0); /* optional */

	flags = SC_ALGORITHM_ECDSA_RAW | SC_ALGORITHM_ECDH_CDH_RAW | SC_ALGORITHM_ECDSA_HASH_NONE;
	ext_flags = SC_ALGORITHM_EXT_EC_NAMEDCURVE | SC_ALGORITHM_EXT_EC_UNCOMPRESES;

	_sc_card_add_ec_alg(card, 256, flags, ext_flags, NULL);
	_sc_card_add_ec_alg(card, 384, flags, ext_flags, NULL);
	_sc_card_add_ec_alg(card, 521, flags, ext_flags, NULL);


	priv = COOLKEY_DATA(card);
	if (priv->pin_count != 0) {
		card->caps |= SC_CARD_CAP_ISO7816_PIN_INFO;
	}

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}