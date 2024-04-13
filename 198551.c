static int cac_parse_CCC(sc_card_t *card, cac_private_data_t *priv, u8 *tl,
						 size_t tl_len, u8 *val, size_t val_len)
{
	size_t len = 0;
	u8 *tl_end = tl + tl_len;
	u8 *val_end = val + val_len;
	sc_path_t new_path;
	int r;


	for (; (tl < tl_end) && (val< val_end); val += len) {
		/* get the tag and the length */
		u8 tag;
		if (sc_simpletlv_read_tag(&tl, tl_end - tl, &tag, &len) != SC_SUCCESS)
			break;
		switch (tag) {
		case CAC_TAG_CUID:
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"TAG:CUID");
			r = cac_parse_cuid(card, priv, (cac_cuid_t *)val, len);
			if (r < 0)
				return r;
			break;
		case CAC_TAG_CC_VERSION_NUMBER:
			if (len != 1) {
				sc_log(card->ctx, "TAG: CC Version: "
				    "Invalid length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			/* ignore the version numbers for now */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
				"TAG: CC Version = 0x%02x", *val);
			break;
		case CAC_TAG_GRAMMAR_VERION_NUMBER:
			if (len != 1) {
				sc_log(card->ctx, "TAG: Grammar Version: "
				    "Invalid length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			/* ignore the version numbers for now */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
				"TAG: Grammar Version = 0x%02x", *val);
			break;
		case CAC_TAG_CARDURL:
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"TAG:CARDURL");
			r = cac_parse_cardurl(card, priv, (cac_card_url_t *)val, len);
			if (r < 0)
				return r;
			break;
		/*
		 * The following are really for file systems cards. This code only cares about CAC VM cards
		 */
		case CAC_TAG_PKCS15:
			if (len != 1) {
				sc_log(card->ctx, "TAG: PKCS15: "
				    "Invalid length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			/* TODO should verify that this is '0'. If it's not
			 * zero, we should drop out of here and let the PKCS 15
			 * code handle this card */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"TAG: PKCS15 = 0x%02x", *val);
			break;
		case CAC_TAG_DATA_MODEL:
		case CAC_TAG_CARD_APDU:
		case CAC_TAG_CAPABILITY_TUPLES:
		case CAC_TAG_STATUS_TUPLES:
		case CAC_TAG_REDIRECTION:
		case CAC_TAG_ERROR_CODES:
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"TAG:FSSpecific(0x%02x)", tag);
			break;
		case CAC_TAG_ACCESS_CONTROL:
			/* TODO handle access control later */
			sc_log_hex(card->ctx, "TAG:ACCESS Control", val, len);
			break;
		case CAC_TAG_NEXT_CCC:
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"TAG:NEXT CCC");
			r = cac_path_from_cardurl(card, &new_path, (cac_card_url_t *)val, len);
			if (r < 0)
				return r;

			r = cac_select_file_by_type(card, &new_path, NULL, SC_CARD_TYPE_CAC_II);
			if (r < 0)
				return r;

			r = cac_process_CCC(card, priv);
			if (r < 0)
				return r;
			break;
		default:
			/* ignore tags we don't understand */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"TAG:Unknown (0x%02x)",tag );
			break;
		}
	}
	return SC_SUCCESS;
}