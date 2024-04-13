static void update_cached_ttl(unsigned char *buf, int len, int new_ttl)
{
	unsigned char *c;
	uint16_t w;
	int l;

	/* skip the header */
	c = buf + 12;
	len -= 12;

	/* skip the query, which is a name and 2 16 bit words */
	l = dns_name_length(c);
	c += l;
	len -= l;
	c += 4;
	len -= 4;

	/* now we get the answer records */

	while (len > 0) {
		/* first a name */
		l = dns_name_length(c);
		c += l;
		len -= l;
		if (len < 0)
			break;
		/* then type + class, 2 bytes each */
		c += 4;
		len -= 4;
		if (len < 0)
			break;

		/* now the 4 byte TTL field */
		c[0] = new_ttl >> 24 & 0xff;
		c[1] = new_ttl >> 16 & 0xff;
		c[2] = new_ttl >> 8 & 0xff;
		c[3] = new_ttl & 0xff;
		c += 4;
		len -= 4;
		if (len < 0)
			break;

		/* now the 2 byte rdlen field */
		w = c[0] << 8 | c[1];
		c += w + 2;
		len -= w + 2;
	}
}