pdf_lookup_cmap(pdf_cmap *cmap, unsigned int cpt)
{
	pdf_range *ranges = cmap->ranges;
	pdf_xrange *xranges = cmap->xranges;
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
			return cpt - ranges[m].low + ranges[m].out;
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
			return cpt - xranges[m].low + xranges[m].out;
	}

	if (cmap->usecmap)
		return pdf_lookup_cmap(cmap->usecmap, cpt);

	return -1;
}