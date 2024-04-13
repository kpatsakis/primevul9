pdf_decode_cmap(pdf_cmap *cmap, unsigned char *buf, unsigned char *end, unsigned int *cpt)
{
	unsigned int c;
	int k, n;
	int len = end - buf;

	if (len > 4)
		len = 4;

	c = 0;
	for (n = 0; n < len; n++)
	{
		c = (c << 8) | buf[n];
		for (k = 0; k < cmap->codespace_len; k++)
		{
			if (cmap->codespace[k].n == n + 1)
			{
				if (c >= cmap->codespace[k].low && c <= cmap->codespace[k].high)
				{
					*cpt = c;
					return n + 1;
				}
			}
		}
	}

	*cpt = 0;
	return 1;
}