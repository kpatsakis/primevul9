static int cac_parse_cuid(sc_card_t *card, cac_private_data_t *priv, cac_cuid_t *val, size_t len)
{
	size_t card_id_len;

	if (len < sizeof(cac_cuid_t)) {
		return SC_ERROR_INVALID_DATA;
	}

	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "gsc_rid=%s", sc_dump_hex(val->gsc_rid, sizeof(val->gsc_rid)));
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "manufacture id=%x", val->manufacturer_id);
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "cac_type=%d", val->card_type);
	card_id_len = len - (&val->card_id - (u8 *)val);
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
		 "card_id=%s (%"SC_FORMAT_LEN_SIZE_T"u)",
		 sc_dump_hex(&val->card_id, card_id_len),
		 card_id_len);
	priv->cuid = *val;
	priv->cac_id = malloc(card_id_len);
	if (priv->cac_id == NULL) {
		return SC_ERROR_OUT_OF_MEMORY;
	}
	memcpy(priv->cac_id, &val->card_id, card_id_len);
	priv->cac_id_len = card_id_len;
	return SC_SUCCESS;
}