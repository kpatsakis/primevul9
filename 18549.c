make_eku_oid(cms_context *cms, SECItem *encoded, SECOidTag oid_tag)
{
	void *rv;
	SECOidData *oid_data;

	oid_data = SECOID_FindOIDByTag(oid_tag);
	if (!oid_data)
		cmsreterr(-1, cms, "could not encode eku oid data");

	rv = SEC_ASN1EncodeItem(cms->arena, encoded, &oid_data->oid,
				EKUOidSequence);
	if (rv == NULL)
		cmsreterr(-1, cms, "could not encode eku oid data");

	encoded->type = siBuffer;
	return 0;
}