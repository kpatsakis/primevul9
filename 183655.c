static int jpc_getnumnewpasses(jpc_bitstream_t *in)
{
	int n;

	if ((n = jpc_bitstream_getbit(in)) > 0) {
		if ((n = jpc_bitstream_getbit(in)) > 0) {
			if ((n = jpc_bitstream_getbits(in, 2)) == 3) {
				if ((n = jpc_bitstream_getbits(in, 5)) == 31) {
					if ((n = jpc_bitstream_getbits(in, 7)) >= 0) {
						n += 36 + 1;
					}
				} else if (n >= 0) {
					n += 5 + 1;
				}
			} else if (n >= 0) {
				n += 2 + 1;
			}
		} else if (!n) {
			n += 2;
		}
	} else if (!n) {
		++n;
	}

	return n;
}