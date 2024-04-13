static int cac_parse_ACA_service(sc_card_t *card, cac_private_data_t *priv,
    u8 *val, size_t val_len)
{
	size_t len = 0;
	u8 *val_end = val + val_len;
	int r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	for (; val < val_end; val += len) {
		/* get the tag and the length */
		u8 tag;
		if (sc_simpletlv_read_tag(&val, val_end - val, &tag, &len) != SC_SUCCESS)
			break;

		switch (tag) {
		case CAC_TAG_APPLET_FAMILY:
			if (len != 5) {
				sc_log(card->ctx, "TAG: Applet Information: "
				    "bad length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Applet Information: Family: 0x%02x", val[0]);
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "     Applet Version: 0x%02x 0x%02x 0x%02x 0x%02x",
			    val[1], val[2], val[3], val[4]);
			break;
		case CAC_TAG_NUMBER_APPLETS:
			if (len != 1) {
				sc_log(card->ctx, "TAG: Num applets: "
				    "bad length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Num applets = %hhd", *val);
			break;
		case CAC_TAG_APPLET_ENTRY:
			/* Make sure we match the outer length */
			if (len < 3 || val[2] != len - 3) {
				sc_log(card->ctx, "TAG: Applet Entry: "
				    "bad length (%"SC_FORMAT_LEN_SIZE_T
				    "u) or length of internal buffer", len);
				break;
			}
			sc_debug_hex(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Applet Entry: AID", &val[3], val[2]);
			/* This is SimpleTLV prefixed with applet ID (1B) */
			r = cac_parse_aid(card, priv, &val[3], val[2]);
			if (r < 0)
				return r;
			break;
		default:
			/* ignore tags we don't understand */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Unknown (0x%02x)", tag);
			break;
		}
	}
	return SC_SUCCESS;
}