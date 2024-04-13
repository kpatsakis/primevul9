generate_spc_string(cms_context *cms, SECItem *ssp, char *str, int len)
{
	SpcString ss;
	memset(&ss, '\0', sizeof (ss));

	SECITEM_AllocItem(cms->arena, &ss.unicode, len);
	if (len != 0) {
		if (!ss.unicode.data)
			cmsreterr(-1, cms, "could not allocate memory");

		memcpy(ss.unicode.data, str, len);
	}
	ss.unicode.type = siBMPString;

	if (SEC_ASN1EncodeItem(cms->arena, ssp, &ss, SpcStringTemplate) == NULL)
		cmsreterr(-1, cms, "could not encode SpcString");

	return 0;
}