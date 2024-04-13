static bool check_buffer(RBinFile *bf, RBuffer *buf) {
	if (r_buf_size (buf) < 32) {
		return false;
	}

	char hdr[17] = {0};
	int rhdr = r_buf_read_at (buf, 0, (ut8 *)&hdr, sizeof (hdr) - 1);
	if (rhdr != sizeof (hdr) - 1) {
		return false;
	}

	return check_magic (hdr);
}