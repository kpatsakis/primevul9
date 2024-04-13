cssp_send_tsrequest(STREAM token, STREAM auth, STREAM pubkey)
{
	STREAM s;
	STREAM h1, h2, h3, h4, h5;

	struct stream tmp = { 0 };
	struct stream message = { 0 };

	memset(&message, 0, sizeof(message));
	memset(&tmp, 0, sizeof(tmp));

	// version [0]
	s_realloc(&tmp, sizeof(uint8));
	s_reset(&tmp);
	out_uint8(&tmp, 2);
	s_mark_end(&tmp);
	h2 = ber_wrap_hdr_data(BER_TAG_INTEGER, &tmp);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h2);
	s_free(h1);

	// negoToken [1]
	if (token && s_length(token))
	{
		h5 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, token);
		h4 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0, h5);
		h3 = ber_wrap_hdr_data(BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, h4);
		h2 = ber_wrap_hdr_data(BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, h3);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 1, h2);
		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));
		s_mark_end(&message);
		s_free(h5);
		s_free(h4);
		s_free(h3);
		s_free(h2);
		s_free(h1);
	}

	// authInfo [2]
	if (auth && s_length(auth))
	{
		h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, auth);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 2, h2);

		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));

		s_free(h2);
		s_free(h1);
	}

	// pubKeyAuth [3]
	if (pubkey && s_length(pubkey))
	{
		h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, pubkey);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 3, h2);

		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));
		s_mark_end(&message);
		s_free(h2);
		s_free(h1);
	}
	s_mark_end(&message);

	// Construct ASN.1 Message
	// Todo: can h1 be send directly instead of tcp_init() approach
	h1 = ber_wrap_hdr_data(BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, &message);
	s = tcp_init(s_length(h1));
	out_uint8p(s, h1->data, s_length(h1));
	s_mark_end(s);
	s_free(h1);

#if WITH_DEBUG_CREDSSP
	streamsave(s, "tsrequest_out.raw");
	printf("Out TSRequest %ld bytes\n", s_length(s));
	hexdump(s->data, s_length(s));
#endif

	tcp_send(s);

	// cleanup
	xfree(message.data);
	xfree(tmp.data);

	return True;
}