generate_string(cms_context *cms, SECItem *der, char *str)
{
	SECItem input;

	input.data = (void *)str;
	input.len = strlen(str);
	input.type = siBMPString;

	void *ret;
	ret = SEC_ASN1EncodeItem(cms->arena, der, &input,
						SEC_PrintableStringTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode string");
	return 0;
}