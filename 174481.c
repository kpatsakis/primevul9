cssp_encode_tspasswordcreds(char *username, char *password, char *domain)
{
	STREAM out, h1, h2;
	struct stream tmp = { 0 };
	struct stream message = { 0 };

	memset(&tmp, 0, sizeof(tmp));
	memset(&message, 0, sizeof(message));

	// domainName [0]
	s_realloc(&tmp, 4 + strlen(domain) * sizeof(uint16));
	s_reset(&tmp);
	rdp_out_unistr(&tmp, domain, strlen(domain) * sizeof(uint16));
	s_mark_end(&tmp);
	h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h2);
	s_free(h1);

	// userName [1]
	s_realloc(&tmp, 4 + strlen(username) * sizeof(uint16));
	s_reset(&tmp);
	rdp_out_unistr(&tmp, username, strlen(username) * sizeof(uint16));
	s_mark_end(&tmp);

	h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 1, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h2);
	s_free(h1);

	// password [2]
	s_realloc(&tmp, 4 + strlen(password) * sizeof(uint16));
	s_reset(&tmp);
	rdp_out_unistr(&tmp, password, strlen(password) * sizeof(uint16));
	s_mark_end(&tmp);
	h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 2, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h2);
	s_free(h1);

	// build message
	out = ber_wrap_hdr_data(BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, &message);

	// cleanup
	xfree(tmp.data);
	xfree(message.data);
	return out;
}