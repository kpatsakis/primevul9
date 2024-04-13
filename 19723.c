cdf_grow_info(cdf_property_info_t **info, size_t *maxcount, size_t incr)
{
	cdf_property_info_t *inp;
	size_t newcount = *maxcount + incr;

	if (newcount > CDF_PROP_LIMIT) {
		DPRINTF(("exceeded property limit %" SIZE_T_FORMAT "u > %"
		    SIZE_T_FORMAT "u\n", newcount, CDF_PROP_LIMIT));
		goto out;
	}
	inp = CAST(cdf_property_info_t *,
	    CDF_REALLOC(*info, newcount * sizeof(*inp)));
	if (inp == NULL)
		goto out;

	*info = inp;
	*maxcount = newcount;
	return inp;
out:
	free(*info);
	*maxcount = 0;
	*info = NULL;
	return NULL;
}