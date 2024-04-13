static char *plist_utf16_to_utf8(uint16_t *unistr, long len, long *items_read, long *items_written)
{
	if (!unistr || (len <= 0)) return NULL;
	char *outbuf;
	int p = 0;
	long i = 0;

	uint16_t wc;
	uint32_t w;
	int read_lead_surrogate = 0;

	outbuf = (char*)malloc(4*(len+1));
	if (!outbuf) {
		PLIST_BIN_ERR("%s: Could not allocate %" PRIu64 " bytes\n", __func__, (uint64_t)(4*(len+1)));
		return NULL;
	}

	while (i < len) {
		wc = unistr[i++];
		if (wc >= 0xD800 && wc <= 0xDBFF) {
			if (!read_lead_surrogate) {
				read_lead_surrogate = 1;
				w = 0x010000 + ((wc & 0x3FF) << 10);
			} else {
				// This is invalid, the next 16 bit char should be a trail surrogate. 
				// Handling error by skipping.
				read_lead_surrogate = 0;
			}
		} else if (wc >= 0xDC00 && wc <= 0xDFFF) {
			if (read_lead_surrogate) {
				read_lead_surrogate = 0;
				w = w | (wc & 0x3FF);
				outbuf[p++] = (char)(0xF0 + ((w >> 18) & 0x7));
				outbuf[p++] = (char)(0x80 + ((w >> 12) & 0x3F));
				outbuf[p++] = (char)(0x80 + ((w >> 6) & 0x3F));
				outbuf[p++] = (char)(0x80 + (w & 0x3F));
			} else {
				// This is invalid.  A trail surrogate should always follow a lead surrogate.
				// Handling error by skipping
			}
		} else if (wc >= 0x800) {
			outbuf[p++] = (char)(0xE0 + ((wc >> 12) & 0xF));
			outbuf[p++] = (char)(0x80 + ((wc >> 6) & 0x3F));
			outbuf[p++] = (char)(0x80 + (wc & 0x3F));
		} else if (wc >= 0x80) {
			outbuf[p++] = (char)(0xC0 + ((wc >> 6) & 0x1F));
			outbuf[p++] = (char)(0x80 + (wc & 0x3F));
		} else {
			outbuf[p++] = (char)(wc & 0x7F);
		}
	}
	if (items_read) {
		*items_read = i;
	}
	if (items_written) {
		*items_written = p;
	}
	outbuf[p] = 0;

	return outbuf;
}