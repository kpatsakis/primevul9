input_osc_parse_colour(const char *p, u_int *r, u_int *g, u_int *b)
{
	u_int		 rsize, gsize, bsize;
	const char	*cp, *s = p;

	if (sscanf(p, "rgb:%x/%x/%x", r, g, b) != 3)
		return (0);
	p += 4;

	cp = strchr(p, '/');
	rsize = cp - p;
	if (rsize == 1)
		(*r) = (*r) | ((*r) << 4);
	else if (rsize == 3)
		(*r) >>= 4;
	else if (rsize == 4)
		(*r) >>= 8;
	else if (rsize != 2)
		return (0);

	p = cp + 1;
	cp = strchr(p, '/');
	gsize = cp - p;
	if (gsize == 1)
		(*g) = (*g) | ((*g) << 4);
	else if (gsize == 3)
		(*g) >>= 4;
	else if (gsize == 4)
		(*g) >>= 8;
	else if (gsize != 2)
		return (0);

	bsize = strlen(cp + 1);
	if (bsize == 1)
		(*b) = (*b) | ((*b) << 4);
	else if (bsize == 3)
		(*b) >>= 4;
	else if (bsize == 4)
		(*b) >>= 8;
	else if (bsize != 2)
		return (0);

	log_debug("%s: %s = %02x%02x%02x", __func__, s, *r, *g, *b);
	return (1);
}