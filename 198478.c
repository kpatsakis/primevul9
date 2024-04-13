static int muscle_init(sc_card_t *card)
{
	muscle_private_t *priv;

	card->name = "MuscleApplet";
	card->drv_data = malloc(sizeof(muscle_private_t));
	if(!card->drv_data) {
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_OUT_OF_MEMORY);
	}
	memset(card->drv_data, 0, sizeof(muscle_private_t));
	priv = MUSCLE_DATA(card);
	priv->verifiedPins = 0;
	priv->fs = mscfs_new();
	if(!priv->fs) {
		free(card->drv_data);
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_OUT_OF_MEMORY);
	}
	priv->fs->udata = card;
	priv->fs->listFile = _listFile;

	card->cla = 0xB0;

	card->flags |= SC_CARD_FLAG_RNG;
	card->caps |= SC_CARD_CAP_RNG;

	/* Card type detection */
	_sc_match_atr(card, muscle_atrs, &card->type);
	if(card->type == SC_CARD_TYPE_MUSCLE_ETOKEN_72K) {
		card->caps |= SC_CARD_CAP_APDU_EXT;
	}
	if(card->type == SC_CARD_TYPE_MUSCLE_JCOP241) {
		card->caps |= SC_CARD_CAP_APDU_EXT;
	}
	if (!(card->caps & SC_CARD_CAP_APDU_EXT)) {
		card->max_recv_size = 255;
		card->max_send_size = 255;
	}
	if(card->type == SC_CARD_TYPE_MUSCLE_JCOP242R2_NO_EXT_APDU) {
	        /* Tyfone JCOP v242R2 card that doesn't support extended APDUs */
	}


	/* FIXME: Card type detection */
	if (1) {
		unsigned long flags;

		flags = SC_ALGORITHM_RSA_RAW;
		flags |= SC_ALGORITHM_RSA_HASH_NONE;
		flags |= SC_ALGORITHM_ONBOARD_KEY_GEN;

		_sc_card_add_rsa_alg(card, 1024, flags, 0);
		_sc_card_add_rsa_alg(card, 2048, flags, 0);
	}
	return SC_SUCCESS;
}