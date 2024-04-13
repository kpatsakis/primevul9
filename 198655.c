epass2003_construct_fci(struct sc_card *card, const sc_file_t * file,
		u8 * out, size_t * outlen)
{
	u8 *p = out;
	u8 buf[64];
	unsigned char ops[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	int rv;
	unsigned ii;

	if (*outlen < 2)
		return SC_ERROR_BUFFER_TOO_SMALL;

	*p++ = 0x62;
	p++;
	if (file->type == SC_FILE_TYPE_WORKING_EF) {
		if (file->ef_structure == SC_FILE_EF_TRANSPARENT) {
			buf[0] = (file->size >> 8) & 0xFF;
			buf[1] = file->size & 0xFF;
			sc_asn1_put_tag(0x80, buf, 2, p, *outlen - (p - out), &p);
		}
	}
	if (file->type == SC_FILE_TYPE_DF) {
		buf[0] = 0x38;
		buf[1] = 0x00;
		sc_asn1_put_tag(0x82, buf, 2, p, *outlen - (p - out), &p);
	}
	else if (file->type == SC_FILE_TYPE_WORKING_EF) {
		buf[0] = file->ef_structure & 7;
		if (file->ef_structure == SC_FILE_EF_TRANSPARENT) {
			buf[1] = 0x00;
			sc_asn1_put_tag(0x82, buf, 2, p, *outlen - (p - out), &p);
		}
		else if (file->ef_structure == SC_FILE_EF_LINEAR_FIXED
			   || file->ef_structure == SC_FILE_EF_LINEAR_VARIABLE) {
			buf[1] = 0x00;
			buf[2] = 0x00;
			buf[3] = 0x40;	/* record length */
			buf[4] = 0x00;	/* record count */
			sc_asn1_put_tag(0x82, buf, 5, p, *outlen - (p - out), &p);
		}
		else {
			return SC_ERROR_NOT_SUPPORTED;
		}

	}
	else if (file->type == SC_FILE_TYPE_INTERNAL_EF) {
		if (file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_CRT ||
			file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_CRT) {
			buf[0] = 0x11;
			buf[1] = 0x00;
		}
		else if (file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC ||
				file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_PUBLIC) {
			buf[0] = 0x12;
			buf[1] = 0x00;
		}
		else {
			return SC_ERROR_NOT_SUPPORTED;
		}
		sc_asn1_put_tag(0x82, buf, 2, p, *outlen - (p - out), &p);
	}
	else if (file->type == SC_FILE_TYPE_BSO) {
		buf[0] = 0x10;
		buf[1] = 0x00;
		sc_asn1_put_tag(0x82, buf, 2, p, *outlen - (p - out), &p);
	}

	buf[0] = (file->id >> 8) & 0xFF;
	buf[1] = file->id & 0xFF;
	sc_asn1_put_tag(0x83, buf, 2, p, *outlen - (p - out), &p);
	if (file->type == SC_FILE_TYPE_DF) {
		if (file->namelen != 0) {
			sc_asn1_put_tag(0x84, file->name, file->namelen, p, *outlen - (p - out), &p);
		}
		else {
			return SC_ERROR_INVALID_ARGUMENTS;
		}
	}
	if (file->type == SC_FILE_TYPE_DF) {
		unsigned char data[2] = {0x00, 0x7F};
		/* 127 files at most */
		sc_asn1_put_tag(0x85, data, sizeof(data), p, *outlen - (p - out), &p);
	}
	else if (file->type == SC_FILE_TYPE_BSO) {
		buf[0] = file->size & 0xff;
		sc_asn1_put_tag(0x85, buf, 1, p, *outlen - (p - out), &p);
	}
	else if (file->type == SC_FILE_TYPE_INTERNAL_EF) {
		if (file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_CRT ||
		    file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC||
		    file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_CRT||
		    file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_PUBLIC) {
			buf[0] = (file->size >> 8) & 0xFF;
			buf[1] = file->size & 0xFF;
			sc_asn1_put_tag(0x85, buf, 2, p, *outlen - (p - out), &p);
		}
	}
	if (file->sec_attr_len) {
		memcpy(buf, file->sec_attr, file->sec_attr_len);
		sc_asn1_put_tag(0x86, buf, file->sec_attr_len, p, *outlen - (p - out), &p);
	}
	else {
		sc_log(card->ctx, "SC_FILE_ACL");
		if (file->type == SC_FILE_TYPE_DF) {
			ops[0] = SC_AC_OP_LIST_FILES;
			ops[1] = SC_AC_OP_CREATE;
			ops[3] = SC_AC_OP_DELETE;
		}
		else if (file->type == SC_FILE_TYPE_WORKING_EF) {
			if (file->ef_structure == SC_FILE_EF_TRANSPARENT) {
				ops[0] = SC_AC_OP_READ;
				ops[1] = SC_AC_OP_UPDATE;
				ops[3] = SC_AC_OP_DELETE;
			}
			else if (file->ef_structure == SC_FILE_EF_LINEAR_FIXED
					|| file->ef_structure == SC_FILE_EF_LINEAR_VARIABLE) {
				ops[0] = SC_AC_OP_READ;
				ops[1] = SC_AC_OP_UPDATE;
				ops[2] = SC_AC_OP_WRITE;
				ops[3] = SC_AC_OP_DELETE;
			}
			else {
				return SC_ERROR_NOT_SUPPORTED;
			}
		}
		else if (file->type == SC_FILE_TYPE_BSO) {
			ops[0] = SC_AC_OP_UPDATE;
			ops[3] = SC_AC_OP_DELETE;
		}
		else if (file->type == SC_FILE_TYPE_INTERNAL_EF) {
			if (file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_CRT ||
				file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_CRT) {
				ops[1] = SC_AC_OP_UPDATE;
				ops[2] = SC_AC_OP_CRYPTO;
				ops[3] = SC_AC_OP_DELETE;
			}
			else if (file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC||
					file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_PUBLIC) {
				ops[0] = SC_AC_OP_READ;
				ops[1] = SC_AC_OP_UPDATE;
				ops[2] = SC_AC_OP_CRYPTO;
				ops[3] = SC_AC_OP_DELETE;
			}
		}
		else {
			return SC_ERROR_NOT_SUPPORTED;
		}

		for (ii = 0; ii < sizeof(ops); ii++) {
			const struct sc_acl_entry *entry;

			buf[ii] = 0xFF;
			if (ops[ii] == 0xFF)
				continue;
			entry = sc_file_get_acl_entry(file, ops[ii]);

			rv = acl_to_ac_byte(card, entry);
			LOG_TEST_RET(card->ctx, rv, "Invalid ACL");

			buf[ii] = rv;
		}
		sc_asn1_put_tag(0x86, buf, sizeof(ops), p, *outlen - (p - out), &p);
		if(file->size == 256)	
		{
			out[4]= 0x13;
		}

	}

	/* VT ??? */
	if (file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC||
		file->ef_structure == SC_CARDCTL_OBERTHUR_KEY_EC_PUBLIC) {
		unsigned char data[2] = {0x00, 0x66};
		sc_asn1_put_tag(0x87, data, sizeof(data), p, *outlen - (p - out), &p);
		if(file->size == 256)	
		{
			out[4]= 0x14;
		}
	}

	out[1] = p - out - 2;

	*outlen = p - out;
	return 0;
}