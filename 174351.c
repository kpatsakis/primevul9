rdp_init_data(int maxlen)
{
	STREAM s;

	s = sec_init(g_encryption ? SEC_ENCRYPT : 0, maxlen + 18);
	s_push_layer(s, rdp_hdr, 18);

	return s;
}