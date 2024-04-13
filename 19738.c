cdf_unpack_catalog(const cdf_header_t *h, const cdf_stream_t *sst,
    cdf_catalog_t **cat)
{
	size_t ss = cdf_check_stream(sst, h);
	const char *b = CAST(const char *, sst->sst_tab);
	const char *nb, *eb = b + ss * sst->sst_len;
	size_t nr, i, j, k;
	cdf_catalog_entry_t *ce;
	uint16_t reclen;
	const uint16_t *np;

	for (nr = 0;; nr++) {
		memcpy(&reclen, b, sizeof(reclen));
		reclen = CDF_TOLE2(reclen);
		if (reclen == 0)
			break;
		b += reclen;
		if (b > eb)
		    break;
	}
	if (nr == 0)
		return -1;
	nr--;
	*cat = CAST(cdf_catalog_t *,
	    CDF_MALLOC(sizeof(cdf_catalog_t) + nr * sizeof(*ce)));
	if (*cat == NULL)
		return -1;
	ce = (*cat)->cat_e;
	memset(ce, 0, nr * sizeof(*ce));
	b = CAST(const char *, sst->sst_tab);
	for (j = i = 0; i < nr; b += reclen) {
		cdf_catalog_entry_t *cep = &ce[j];
		uint16_t rlen;

		extract_catalog_field(uint16_t, ce_namlen, 0);
		extract_catalog_field(uint16_t, ce_num, 4);
		extract_catalog_field(uint64_t, ce_timestamp, 8);
		reclen = cep->ce_namlen;

		if (reclen < 14) {
			cep->ce_namlen = 0;
			continue;
		}

		cep->ce_namlen = __arraycount(cep->ce_name) - 1;
		rlen = reclen - 14;
		if (cep->ce_namlen > rlen)
			cep->ce_namlen = rlen;

		np = CAST(const uint16_t *, CAST(const void *, (b + 16)));
		nb = CAST(const char *, CAST(const void *,
		    (np + cep->ce_namlen)));
		if (nb > eb) {
			cep->ce_namlen = 0;
			break;
		}

		for (k = 0; k < cep->ce_namlen; k++)
			cep->ce_name[k] = np[k]; /* XXX: CDF_TOLE2? */
		cep->ce_name[cep->ce_namlen] = 0;
		j = i;
		i++;
	}
	(*cat)->cat_num = j;
	return 0;
}