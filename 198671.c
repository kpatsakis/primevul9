static int cac_parse_properties_object(sc_card_t *card, u8 type,
    u8 *data, size_t data_len, cac_properties_object_t *object)
{
	size_t len;
	u8 *val, *val_end, tag;
	int parsed = 0;

	if (data_len < 11)
		return -1;

	/* Initilize: non-PKI applet */
	object->privatekey = 0;

	val = data;
	val_end = data + data_len;
	for (; val < val_end; val += len) {
		/* get the tag and the length */
		if (sc_simpletlv_read_tag(&val, val_end - val, &tag, &len) != SC_SUCCESS)
			break;

		switch (tag) {
		case CAC_TAG_OBJECT_ID:
			if (len != 2) {
				sc_log(card->ctx, "TAG: Object ID: "
				    "Invalid length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Object ID = 0x%02x 0x%02x", val[0], val[1]);
			memcpy(&object->oid, val, 2);
			parsed++;
			break;

		case CAC_TAG_BUFFER_PROPERTIES:
			if (len != 5) {
				sc_log(card->ctx, "TAG: Buffer Properties: "
				    "Invalid length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			/* First byte is "Type of Tag Supported" */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Buffer Properties: Type of Tag Supported = 0x%02x",
			    val[0]);
			object->simpletlv = val[0];
			parsed++;
			break;

		case CAC_TAG_PKI_PROPERTIES:
			/* 4th byte is "Private Key Initialized" */
			if (len != 4) {
				sc_log(card->ctx, "TAG: PKI Properties: "
				    "Invalid length %"SC_FORMAT_LEN_SIZE_T"u", len);
				break;
			}
			if (type != CAC_TAG_PKI_OBJECT) {
				sc_log(card->ctx, "TAG: PKI Properties outside of PKI Object");
				break;
			}
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: PKI Properties: Private Key Initialized = 0x%02x",
			    val[2]);
			object->privatekey = val[2];
			parsed++;
			break;

		default:
			/* ignore tags we don't understand */
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
			    "TAG: Unknown (0x%02x)",tag );
			break;
		}
	}
	if (parsed < 2)
		return SC_ERROR_INVALID_DATA;

	return SC_SUCCESS;
}