int blkid_known_pttype(const char *pttype)
{
	size_t i;

	if (!pttype)
		return 0;

	for (i = 0; i < ARRAY_SIZE(idinfos); i++) {
		const struct blkid_idinfo *id = idinfos[i];
		if (strcmp(id->name, pttype) == 0)
			return 1;
	}
	return 0;
}