pdf_lookup_cmap_full(pdf_cmap *cmap, unsigned int cpt, int *out)
{
	pdf_range *ranges = cmap->ranges;
	pdf_xrange *xranges = cmap->xranges;
	pdf_mrange *mranges = cmap->mranges;
	unsigned int i;
	int l, r, m;

	l = 0;
	r = cmap->rlen - 1;
	while (l <= r)
	{
		m = (l + r) >> 1;
		if (cpt < ranges[m].low)
			r = m - 1;
		else if (cpt > ranges[m].high)
			l = m + 1;
		else
		{
			out[0] = cpt - ranges[m].low + ranges[m].out;
			return 1;
		}
	}

	l = 0;
	r = cmap->xlen - 1;
	while (l <= r)
	{
		m = (l + r) >> 1;
		if (cpt < xranges[m].low)
			r = m - 1;
		else if (cpt > xranges[m].high)
			l = m + 1;
		else
		{
			out[0] = cpt - xranges[m].low + xranges[m].out;
			return 1;
		}
	}

	l = 0;
	r = cmap->mlen - 1;
	while (l <= r)
	{
		m = (l + r) >> 1;
		if (cpt < mranges[m].low)
			r = m - 1;
		else if (cpt > mranges[m].low)
			l = m + 1;
		else
		{
			int *ptr = &cmap->dict[cmap->mranges[m].out];
			unsigned int len = (unsigned int)*ptr++;
			for (i = 0; i < len; ++i)
				out[i] = *ptr++;
			return len;
		}
	}

	if (cmap->usecmap)
		return pdf_lookup_cmap_full(cmap->usecmap, cpt, out);

	return 0;
}