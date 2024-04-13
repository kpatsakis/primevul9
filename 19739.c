cdf_print_property_name(char *buf, size_t bufsiz, uint32_t p)
{
	size_t i;

	for (i = 0; i < __arraycount(vn); i++)
		if (vn[i].v == p)
			return snprintf(buf, bufsiz, "%s", vn[i].n);
	return snprintf(buf, bufsiz, "%#x", p);
}