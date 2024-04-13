ptp_unpack_EOS_CustomFuncEx (PTPParams* params, unsigned char** data )
{
	uint32_t s = dtoh32a( *data );
	uint32_t n = s/4, i;
	char	*str, *p;

	if (s > 1024) {
		ptp_debug (params, "customfuncex data is larger than 1k / %d... unexpected?", s);
		return strdup("bad length");
	}
	str = (char*)malloc( s*2+s/4+1 ); /* n is size in uint32, maximum %x len is 8 chars and \0*/
	if (!str)
		return strdup("malloc failed");

	p = str;
	for (i=0; i < n; ++i)
		p += sprintf(p, "%x,", dtoh32a( *data + 4*i ));
	return str;
}