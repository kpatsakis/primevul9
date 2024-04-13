generate_auth_info(cms_context *cms, SECItem *der, char *url)
{
	AuthInfo ai;

	SECOidData *oid = SECOID_FindOIDByTag(SEC_OID_PKIX_CA_ISSUERS);
	if (!oid)
		cmsreterr(-1, cms, "could not get CA issuers OID");

	memcpy(&ai.oid, &oid->oid, sizeof (ai.oid));

	SECItem urlitem = {
		.data = (unsigned char *)url,
		.len = strlen(url),
		.type = siBuffer
	};
	int rc = make_context_specific(cms, 6, &ai.url, &urlitem);
	if (rc < 0)
		return rc;

	void *ret;
	SECItem unwrapped;
	ret = SEC_ASN1EncodeItem(cms->arena, &unwrapped, &ai, AuthInfoTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode CA Issuers");

	rc = wrap_in_seq(cms, der, &unwrapped, 1);
	if (rc < 0)
		return rc;
	return 0;

	/* I've no idea how to get SEC_ASN1EncodeItem to spit out the thing
	 * we actually want here.  So once again, just force the data to
	 * look correct :( */
	if (unwrapped.len < 12) {
		cms->log(cms, LOG_ERR, "%s:%s:%d generated CA Issuers Info "
			"cannot possibly be valid",
			__FILE__, __func__, __LINE__);
		return -1;
	}
	unwrapped.data[12] = 0x86;
	unwrapped.type = siBuffer;

	AuthInfo wrapper;
	oid = SECOID_FindOIDByTag(SEC_OID_X509_AUTH_INFO_ACCESS);
	if (!oid)
		cmsreterr(-1, cms, "could not find Auth Info Access OID");

	memcpy(&wrapper.oid, &oid->oid, sizeof (ai.oid));

	wrap_in_seq(cms, &wrapper.url, &unwrapped, 1);

	ret = SEC_ASN1EncodeItem(cms->arena, der, &wrapper,
					AuthInfoWrapperTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode CA Issuers OID");

	return 0;
}