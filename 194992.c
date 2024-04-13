sc_oberthur_parse_publicinfo (struct sc_pkcs15_card *p15card,
		unsigned char *buff, size_t len, int postpone_allowed)
{
	struct sc_context *ctx = p15card->card->ctx;
	size_t ii;
	int rv;

	LOG_FUNC_CALLED(ctx);
	for (ii=0; ii<len; ii+=5)   {
		unsigned int file_id, size;

		if(*(buff+ii) != 0xFF)
			continue;

		file_id = 0x100 * *(buff+ii + 1) + *(buff+ii + 2);
		size = 0x100 * *(buff+ii + 3) + *(buff+ii + 4);
		sc_log(ctx, "add public object(file-id:%04X,size:%X)", file_id, size);

		switch (*(buff+ii + 1))   {
		case BASE_ID_PUB_RSA :
			rv = sc_pkcs15emu_oberthur_add_pubkey(p15card, file_id, size);
			LOG_TEST_RET(ctx, rv, "Cannot parse public key info");
			break;
		case BASE_ID_CERT :
			rv = sc_pkcs15emu_oberthur_add_cert(p15card, file_id);
			LOG_TEST_RET(ctx, rv, "Cannot parse certificate info");
			break;
		case BASE_ID_PUB_DES :
			break;
		case BASE_ID_PUB_DATA :
			rv = sc_pkcs15emu_oberthur_add_data(p15card, file_id, size, 0);
			LOG_TEST_RET(ctx, rv, "Cannot parse data info");
			break;
		default:
			LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Public object parse error");
		}
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}