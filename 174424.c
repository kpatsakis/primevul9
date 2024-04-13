mcs_send_connect_initial(STREAM mcs_data)
{
	int datalen = mcs_data->end - mcs_data->data;
	int length = 9 + 3 * 34 + 4 + datalen;
	STREAM s;

	s = iso_init(length + 5);

	ber_out_header(s, MCS_CONNECT_INITIAL, length);
	ber_out_header(s, BER_TAG_OCTET_STRING, 1);	/* calling domain */
	out_uint8(s, 1);
	ber_out_header(s, BER_TAG_OCTET_STRING, 1);	/* called domain */
	out_uint8(s, 1);

	ber_out_header(s, BER_TAG_BOOLEAN, 1);
	out_uint8(s, 0xff);	/* upward flag */

	mcs_out_domain_params(s, 34, 2, 0, 0xffff);	/* target params */
	mcs_out_domain_params(s, 1, 1, 1, 0x420);	/* min params */
	mcs_out_domain_params(s, 0xffff, 0xfc17, 0xffff, 0xffff);	/* max params */

	ber_out_header(s, BER_TAG_OCTET_STRING, datalen);
	out_uint8p(s, mcs_data->data, datalen);

	s_mark_end(s);
	iso_send(s);
}