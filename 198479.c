size_t sc_right_trim(u8 *buf, size_t len) {

	size_t i;

	if (!buf)
		return 0;

	if (len > 0) {
		for(i = len-1; i > 0; i--) {
			if(!isprint(buf[i])) {
				buf[i] = '\0';
				len--;
				continue;
			}
			break;
		}
	}
	return len;
}