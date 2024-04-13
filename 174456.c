rdp_protocol_error(const char *message, STREAM s)
{
	error("%s(), %s", __func__, message);
	if (s)
		hexdump(s->p, s_length(s));
	exit(0);
}