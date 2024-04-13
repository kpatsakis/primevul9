wrap_in_set(cms_context *cms, SECItem *der, SECItem **items)
{
	void *ret;

	ret = SEC_ASN1EncodeItem(cms->arena, der, &items, &SetTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode set");
	return 0;
}