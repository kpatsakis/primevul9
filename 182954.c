isspf(const dns_rdata_t *rdata) {
	char buf[1024];
	const unsigned char *data = rdata->data;
	unsigned int rdl = rdata->length, i = 0, tl, len;

	while (rdl > 0U) {
		len = tl = *data;
		++data;
		--rdl;
		INSIST(tl <= rdl);
		if (len > sizeof(buf) - i - 1)
			len = sizeof(buf) - i - 1;
		memmove(buf + i, data, len);
		i += len;
		data += tl;
		rdl -= tl;
	}

	if (i < 6U)
		return(false);

	buf[i] = 0;
	if (strncmp(buf, "v=spf1", 6) == 0 && (buf[6] == 0 || buf[6] == ' '))
		return (true);
	return (false);
}