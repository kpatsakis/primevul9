static int jpc_getcommacode(jpc_bitstream_t *in)
{
	int n;
	int v;

	n = 0;
	for (;;) {
		if ((v = jpc_bitstream_getbit(in)) < 0) {
			return -1;
		}
		if (jpc_bitstream_eof(in)) {
			return -1;
		}
		if (!v) {
			break;
		}
		++n;
	}

	return n;
}