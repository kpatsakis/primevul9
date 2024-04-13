sec_establish_key(void)
{
	uint32 length = g_server_public_key_len + SEC_PADDING_SIZE;
	uint32 flags = SEC_CLIENT_RANDOM;
	STREAM s;

	s = sec_init(flags, length + 4);

	out_uint32_le(s, length);
	out_uint8p(s, g_sec_crypted_random, g_server_public_key_len);
	out_uint8s(s, SEC_PADDING_SIZE);

	s_mark_end(s);
	sec_send(s, flags);
}