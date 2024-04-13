sec_init(uint32 flags, int maxlen)
{
	int hdrlen;
	STREAM s;

	if (!g_licence_issued && !g_licence_error_result)
		hdrlen = (flags & SEC_ENCRYPT) ? 12 : 4;
	else
		hdrlen = (flags & SEC_ENCRYPT) ? 12 : 0;
	s = mcs_init(maxlen + hdrlen);
	s_push_layer(s, sec_hdr, hdrlen);

	return s;
}