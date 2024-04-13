long jpc_dec_lookahead(jas_stream_t *in)
{
	uint_fast16_t x;
	if (jpc_getuint16(in, &x)) {
		return -1;
	}
	if (jas_stream_ungetc(in, x & 0xff) == EOF ||
	  jas_stream_ungetc(in, x >> 8) == EOF) {
		return -1;
	}
	if (x >= JPC_MS_INMIN && x <= JPC_MS_INMAX) {
		return x;
	}
	return 0;
}