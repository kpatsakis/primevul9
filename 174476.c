cssp_encode_tscredentials(char *username, char *password, char *domain)
{
	STREAM out;
	STREAM h1, h2, h3;
	struct stream tmp = { 0 };
	struct stream message = { 0 };

	// credType [0]
	s_realloc(&tmp, sizeof(uint8));
	s_reset(&tmp);
	if (g_use_password_as_pin == False)
	{
		out_uint8(&tmp, 1);	// TSPasswordCreds
	}
	else
	{
		out_uint8(&tmp, 2);	// TSSmartCardCreds
	}

	s_mark_end(&tmp);
	h2 = ber_wrap_hdr_data(BER_TAG_INTEGER, &tmp);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h2);
	s_free(h1);

	// credentials [1]
	if (g_use_password_as_pin == False)
	{
		h3 = cssp_encode_tspasswordcreds(username, password, domain);
	}
	else
	{
		h3 = cssp_encode_tssmartcardcreds(username, password, domain);
	}

	h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, h3);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 1, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h3);
	s_free(h2);
	s_free(h1);

	// Construct ASN.1 message
	out = ber_wrap_hdr_data(BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, &message);

#if WITH_DEBUG_CREDSSP
	streamsave(out, "tscredentials.raw");
	printf("Out TSCredentials %ld bytes\n", s_length(out));
	hexdump(out->data, s_length(out));
#endif

	// cleanup
	xfree(message.data);
	xfree(tmp.data);

	return out;
}