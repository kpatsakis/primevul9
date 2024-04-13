const u8 *sc_compacttlv_find_tag(const u8 *buf, size_t len, u8 tag, size_t *outlen)
{
	if (buf != NULL) {
		size_t idx;
		u8 plain_tag = tag & 0xF0;
		size_t expected_len = tag & 0x0F;

	        for (idx = 0; idx < len; idx++) {
			if ((buf[idx] & 0xF0) == plain_tag && idx + expected_len < len &&
			    (expected_len == 0 || expected_len == (buf[idx] & 0x0F))) {
				if (outlen != NULL)
					*outlen = buf[idx] & 0x0F;
				return buf + (idx + 1);
			}
			idx += (buf[idx] & 0x0F);
                }
        }
	return NULL;
}