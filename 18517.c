generate_validity(cms_context *cms, SECItem *der, time_t start, time_t end)
{
	Validity validity;
	int rc;

	rc = generate_time(cms, &validity.start, start);
	if (rc < 0)
		return rc;

	rc = generate_time(cms, &validity.end, end);
	if (rc < 0)
		return rc;

	void *ret;
	ret = SEC_ASN1EncodeItem(cms->arena, der, &validity, ValidityTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode validity");
	return 0;
}