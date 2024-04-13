epass2003_process_fci(struct sc_card *card, sc_file_t * file, const u8 * buf, size_t buflen)
{
	sc_context_t *ctx = card->ctx;
	size_t taglen, len = buflen;
	const u8 *tag = NULL, *p = buf;

	sc_log(ctx, "processing FCI bytes");
	tag = sc_asn1_find_tag(ctx, p, len, 0x83, &taglen);
	if (tag != NULL && taglen == 2) {
		file->id = (tag[0] << 8) | tag[1];
		sc_log(ctx, "  file identifier: 0x%02X%02X", tag[0], tag[1]);
	}

	tag = sc_asn1_find_tag(ctx, p, len, 0x80, &taglen);
	if (tag != NULL && taglen > 0 && taglen < 3) {
		file->size = tag[0];
		if (taglen == 2)
			file->size = (file->size << 8) + tag[1];
		sc_log(ctx, "  bytes in file: %"SC_FORMAT_LEN_SIZE_T"u",
		       file->size);
	}

	if (tag == NULL) {
		tag = sc_asn1_find_tag(ctx, p, len, 0x81, &taglen);
		if (tag != NULL && taglen >= 2) {
			int bytes = (tag[0] << 8) + tag[1];

			sc_log(ctx, "  bytes in file: %d", bytes);
			file->size = bytes;
		}
	}

	tag = sc_asn1_find_tag(ctx, p, len, 0x82, &taglen);
	if (tag != NULL) {
		if (taglen > 0) {
			unsigned char byte = tag[0];
			const char *type;

			if (byte == 0x38) {
				type = "DF";
				file->type = SC_FILE_TYPE_DF;
			}
			else if (0x01 <= byte && byte <= 0x07) {
				type = "working EF";
				file->type = SC_FILE_TYPE_WORKING_EF;
				switch (byte) {
				case 0x01:
					file->ef_structure = SC_FILE_EF_TRANSPARENT;
					break;
				case 0x02:
					file->ef_structure = SC_FILE_EF_LINEAR_FIXED;
					break;
				case 0x04:
					file->ef_structure = SC_FILE_EF_LINEAR_FIXED;
					break;
				case 0x03:
				case 0x05:
				case 0x06:
				case 0x07:
					break;
				default:
					break;
				}

			}
			else if (0x10 == byte) {
				type = "BSO";
				file->type = SC_FILE_TYPE_BSO;
			}
			else if (0x11 <= byte) {
				type = "internal EF";
				file->type = SC_FILE_TYPE_INTERNAL_EF;
				switch (byte) {
				case 0x11:
					break;
				case 0x12:
					break;
				default:
					break;
				}
			}
			else {
				type = "unknown";
				file->type = SC_FILE_TYPE_INTERNAL_EF;

			}
			sc_log(ctx, "type %s, EF structure %d", type, byte);
		}
	}

	tag = sc_asn1_find_tag(ctx, p, len, 0x84, &taglen);
	if (tag != NULL && taglen > 0 && taglen <= 16) {
		memcpy(file->name, tag, taglen);
		file->namelen = taglen;

		sc_log_hex(ctx, "File name", file->name, file->namelen);
		if (!file->type)
			file->type = SC_FILE_TYPE_DF;
	}

	tag = sc_asn1_find_tag(ctx, p, len, 0x85, &taglen);
	if (tag != NULL && taglen)
		sc_file_set_prop_attr(file, tag, taglen);
	else
		file->prop_attr_len = 0;

	tag = sc_asn1_find_tag(ctx, p, len, 0xA5, &taglen);
	if (tag != NULL && taglen)
		sc_file_set_prop_attr(file, tag, taglen);

	tag = sc_asn1_find_tag(ctx, p, len, 0x86, &taglen);
	if (tag != NULL && taglen)
		sc_file_set_sec_attr(file, tag, taglen);

	tag = sc_asn1_find_tag(ctx, p, len, 0x8A, &taglen);
	if (tag != NULL && taglen == 1) {
		if (tag[0] == 0x01)
			file->status = SC_FILE_STATUS_CREATION;
		else if (tag[0] == 0x07 || tag[0] == 0x05)
			file->status = SC_FILE_STATUS_ACTIVATED;
		else if (tag[0] == 0x06 || tag[0] == 0x04)
			file->status = SC_FILE_STATUS_INVALIDATED;
	}
	file->magic = SC_FILE_MAGIC;

	return 0;
}