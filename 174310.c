cssp_encode_tscspdatadetail(unsigned char keyspec, char *card, char *reader, char *container,
			    char *csp)
{
	STREAM out;
	STREAM h1, h2;
	struct stream tmp = { 0 };
	struct stream message = { 0 };

	// keySpec [0]
	s_realloc(&tmp, sizeof(uint8));
	s_reset(&tmp);
	out_uint8(&tmp, keyspec);
	s_mark_end(&tmp);
	h2 = ber_wrap_hdr_data(BER_TAG_INTEGER, &tmp);
	h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0, h2);
	s_realloc(&message, s_length(&message) + s_length(h1));
	out_uint8p(&message, h1->data, s_length(h1));
	s_mark_end(&message);
	s_free(h2);
	s_free(h1);

	// cardName [1]
	if (card)
	{
		s_realloc(&tmp, 4 + strlen(card) * sizeof(uint16));
		s_reset(&tmp);
		rdp_out_unistr(&tmp, card, strlen(card) * sizeof(uint16));
		s_mark_end(&tmp);
		h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 1, h2);
		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));
		s_mark_end(&message);
		s_free(h2);
		s_free(h1);
	}

	// readerName [2]
	if (reader)
	{
		s_realloc(&tmp, 4 + strlen(reader) * sizeof(uint16));
		s_reset(&tmp);
		rdp_out_unistr(&tmp, reader, strlen(reader) * sizeof(uint16));
		s_mark_end(&tmp);
		h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 2, h2);
		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));
		s_mark_end(&message);
		s_free(h2);
		s_free(h1);
	}

	// containerName [3]
	if (container)
	{
		s_realloc(&tmp, 4 + strlen(container) * sizeof(uint16));
		s_reset(&tmp);
		rdp_out_unistr(&tmp, container, strlen(container) * sizeof(uint16));
		s_mark_end(&tmp);
		h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 3, h2);
		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));
		s_mark_end(&message);
		s_free(h2);
		s_free(h1);
	}

	// cspName [4]
	if (csp)
	{
		s_realloc(&tmp, 4 + strlen(csp) * sizeof(uint16));
		s_reset(&tmp);
		rdp_out_unistr(&tmp, csp, strlen(csp) * sizeof(uint16));
		s_mark_end(&tmp);
		h2 = ber_wrap_hdr_data(BER_TAG_OCTET_STRING, &tmp);
		h1 = ber_wrap_hdr_data(BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 4, h2);
		s_realloc(&message, s_length(&message) + s_length(h1));
		out_uint8p(&message, h1->data, s_length(h1));
		s_mark_end(&message);
		s_free(h2);
		s_free(h1);
	}

	s_mark_end(&message);

	// build message
	out = ber_wrap_hdr_data(BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, &message);

	// cleanup
	free(tmp.data);
	free(message.data);
	return out;
}