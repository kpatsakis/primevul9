generate_object_id(cms_context *cms, SECItem *der, SECOidTag tag)
{
	SECOidData *oid;

	oid = SECOID_FindOIDByTag(tag);
	if (!oid)
		cmsreterr(-1, cms, "could not find OID");

	void *ret;
	ret = SEC_ASN1EncodeItem(cms->arena, der, &oid->oid,
						SEC_ObjectIDTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode ODI");
	return 0;
}