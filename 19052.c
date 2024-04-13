static bool check_buffer(RzBuffer *buf) {
	ut8 tmp[6];
	int r = rz_buf_read_at(buf, 0, tmp, sizeof(tmp));
	return r == sizeof(tmp) && !memcmp(tmp, QNX_MAGIC, sizeof(tmp));
}