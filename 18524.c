encode_algorithm_id(cms_context *cms, SECItem *der, SECOidTag tag)
{
	SECAlgorithmID id;

	int rc = generate_algorithm_id(cms, &id, tag);
	if (rc < 0)
		return rc;

	void *ret;
	ret = SEC_ASN1EncodeItem(cms->arena, der, &id,
						SECOID_AlgorithmIDTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode Algorithm ID");

	return 0;
}