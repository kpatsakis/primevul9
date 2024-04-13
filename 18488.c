generate_authvar_signer_info(cms_context *cms, SpcSignerInfo *sip)
{
	SpcSignerInfo si;
	SECItem buf;

	if (!sip)
		return -1;

	memset(&si, '\0', sizeof (si));

	if (SEC_ASN1EncodeInteger(cms->arena, &si.CMSVersion, 1) == NULL) {
		cms->log(cms, LOG_ERR, "could not encode CMSVersion: %s",
			PORT_ErrorToString(PORT_GetError()));
		goto err;
	}

	si.sid.signerType = signerTypeIssuerAndSerialNumber;
	si.sid.signerValue.iasn.issuer = cms->cert->derIssuer;
	si.sid.signerValue.iasn.serial = cms->cert->serialNumber;

	if (generate_algorithm_id(cms, &si.digestAlgorithm,
			digest_get_digest_oid(cms)) < 0)
		goto err;

	si.signedAttrs.len = 0;
	si.signedAttrs.data = NULL;

	buf.len = cms->authbuf_len;
	buf.data = cms->authbuf;
	if (sign_blob(cms, &si.signature, &buf) < 0)
		goto err;

	if (generate_algorithm_id(cms, &si.signatureAlgorithm,
				digest_get_encryption_oid(cms)) < 0)
		goto err;

	si.unsignedAttrs.len = 0;
	si.unsignedAttrs.data = NULL;

	memcpy(sip, &si, sizeof(si));
	return 0;
err:
	return -1;
}