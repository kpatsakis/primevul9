mcs_out_domain_params(STREAM s, int max_channels, int max_users, int max_tokens, int max_pdusize)
{
	ber_out_header(s, MCS_TAG_DOMAIN_PARAMS, 32);
	ber_out_integer(s, max_channels);
	ber_out_integer(s, max_users);
	ber_out_integer(s, max_tokens);
	ber_out_integer(s, 1);	/* num_priorities */
	ber_out_integer(s, 0);	/* min_throughput */
	ber_out_integer(s, 1);	/* max_height */
	ber_out_integer(s, max_pdusize);
	ber_out_integer(s, 2);	/* ver_protocol */
}