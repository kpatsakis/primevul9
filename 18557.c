generate_spc_signer_info(cms_context *cms, SpcSignerInfo *sip)
{
	if (!sip)
		return -1;

	SpcSignerInfo si;
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


	if (cms->raw_signature) {
		memcpy(&si.signedAttrs, cms->raw_signed_attrs,
			sizeof (si.signedAttrs));
		memcpy(&si.signature, cms->raw_signature, sizeof(si.signature));
	} else {
		if (generate_signed_attributes(cms, &si.signedAttrs) < 0)
			goto err;

		if (sign_blob(cms, &si.signature, &si.signedAttrs) < 0)
			goto err;
	}

	si.signedAttrs.data[0] = SEC_ASN1_CONTEXT_SPECIFIC | 0 |
				SEC_ASN1_CONSTRUCTED;

	if (generate_algorithm_id(cms, &si.signatureAlgorithm,
				digest_get_encryption_oid(cms)) < 0)
		goto err;

	if (generate_unsigned_attributes(cms, &si.unsignedAttrs) < 0)
		goto err;

	memcpy(sip, &si, sizeof(si));
	return 0;
err:
	return -1;
}