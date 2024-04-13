epass2003_init(struct sc_card *card)
{
	unsigned int flags;
	unsigned int ext_flags;
	unsigned char data[SC_MAX_APDU_BUFFER_SIZE] = { 0 };
	size_t datalen = SC_MAX_APDU_BUFFER_SIZE;
	epass2003_exdata *exdata = NULL;

	LOG_FUNC_CALLED(card->ctx);

	card->name = "epass2003";
	card->cla = 0x00;
	exdata = (epass2003_exdata *)calloc(1, sizeof(epass2003_exdata));
	if (!exdata)
		return SC_ERROR_OUT_OF_MEMORY;

	card->drv_data = exdata;

	exdata->sm = SM_SCP01;

	/* decide FIPS/Non-FIPS mode */
	if (SC_SUCCESS != get_data(card, 0x86, data, datalen))
		return SC_ERROR_INVALID_CARD;

	if (0x01 == data[2])
		exdata->smtype = KEY_TYPE_AES;
	else
		exdata->smtype = KEY_TYPE_DES;

	if (0x84 == data[14]) {
		if (0x00 == data[16]) { 
			exdata->sm = SM_PLAIN;
		}
	}


	/* mutual authentication */
	card->max_recv_size = 0xD8;
	card->max_send_size = 0xE8;

	card->sm_ctx.ops.open = epass2003_refresh;
	card->sm_ctx.ops.get_sm_apdu = epass2003_sm_get_wrapped_apdu;
	card->sm_ctx.ops.free_sm_apdu = epass2003_sm_free_wrapped_apdu;

	/* FIXME (VT): rather then set/unset 'g_sm', better to implement filter for APDUs to be wrapped */
	epass2003_refresh(card);

	card->sm_ctx.sm_mode = SM_MODE_TRANSMIT;

	flags = SC_ALGORITHM_ONBOARD_KEY_GEN | SC_ALGORITHM_RSA_RAW | SC_ALGORITHM_RSA_HASH_NONE;

	_sc_card_add_rsa_alg(card, 512, flags, 0);
	_sc_card_add_rsa_alg(card, 768, flags, 0);
	_sc_card_add_rsa_alg(card, 1024, flags, 0);
	_sc_card_add_rsa_alg(card, 2048, flags, 0);

	//set EC Alg Flags
	flags = SC_ALGORITHM_ONBOARD_KEY_GEN|SC_ALGORITHM_ECDSA_HASH_SHA1|SC_ALGORITHM_ECDSA_HASH_SHA256|SC_ALGORITHM_ECDSA_HASH_NONE|SC_ALGORITHM_ECDSA_RAW;
	ext_flags = 0;
	_sc_card_add_ec_alg(card, 256, flags, ext_flags, NULL);

	card->caps = SC_CARD_CAP_RNG | SC_CARD_CAP_APDU_EXT;

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}