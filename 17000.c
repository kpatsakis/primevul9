cdf_clsid_to_mime(const uint64_t clsid[2], const struct cv *cv)
{
	size_t i;
	for (i = 0; cv[i].mime != NULL; i++) {
	printf("%llx %llx %llx %llx\n",
		clsid[0], cv[i].clsid[0], clsid[1], cv[i].clsid[1]);
		if (clsid[0] == cv[i].clsid[0] && clsid[1] == cv[i].clsid[1])
			return cv[i].mime;
	}
	return NULL;
}