generate_common_name(cms_context *cms, SECItem *der, char *cn_str)
{
	CommonName cn;
	SECItem cn_item;
	int rc;

	rc = generate_object_id(cms, &cn.oid, SEC_OID_AVA_COMMON_NAME);
	if (rc < 0)
		return rc;
	rc = generate_string(cms, &cn.string, cn_str);
	if (rc < 0)
		return rc;

	void *ret;
	ret = SEC_ASN1EncodeItem(cms->arena, &cn_item, &cn, CommonNameTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode common name");

	SECItem cn_set;
	SECItem *items[2] = {&cn_item, NULL};
	rc = wrap_in_set(cms, &cn_set, items);
	if (rc < 0)
		return rc;
	rc = wrap_in_seq(cms, der, &cn_set, 1);
	if (rc < 0)
		return rc;
	return 0;
}