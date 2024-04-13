char *csr_buildidtostr(uint16_t id)
{
	static char str[12];
	int i;

	for (i = 0; csr_map[i].id; i++)
		if (csr_map[i].id == id)
			return csr_map[i].str;

	snprintf(str, 11, "Build %d", id);
	return str;
}