generate_signed_attributes(cms_context *cms, SECItem *sattrs)
{
	Attribute *attrs[5];
	memset(attrs, '\0', sizeof (attrs));

	SECItem encoded;
	SECOidTag tag;
	SECOidData *oid;

	/* build the first attribute, which says we have no S/MIME
	 * capabilities whatsoever */
	attrs[0] = PORT_ArenaZAlloc(cms->arena, sizeof (Attribute));
	if (!attrs[0])
		goto err;

	oid = SECOID_FindOIDByTag(SEC_OID_PKCS9_SMIME_CAPABILITIES);
	attrs[0]->attrType = oid->oid;

	SECItem *smime_caps[2] = { NULL, NULL};
	if (generate_empty_sequence(cms, &encoded) < 0)
		goto err;
	smime_caps[0] = SECITEM_ArenaDupItem(cms->arena, &encoded);
	attrs[0]->attrValues = smime_caps;

	/* build the second attribute, which says that this is
	 * a PKCS9 content blob thingy */
	attrs[1] = PORT_ArenaZAlloc(cms->arena, sizeof (Attribute));
	if (!attrs[1])
		goto err;

	oid = SECOID_FindOIDByTag(SEC_OID_PKCS9_CONTENT_TYPE);
	attrs[1]->attrType = oid->oid;

	SECItem *content_types[2] = { NULL, NULL };
	tag = find_ms_oid_tag(SPC_INDIRECT_DATA_OBJID);
	if (tag == SEC_OID_UNKNOWN)
		goto err;
	if (generate_object_id(cms, &encoded, tag) < 0)
		goto err;
	content_types[0] = SECITEM_ArenaDupItem(cms->arena, &encoded);
	if (!content_types[0])
		goto err;
	attrs[1]->attrValues = content_types;

	/* build the third attribute.  This is our signing time. */
	attrs[2] = PORT_ArenaZAlloc(cms->arena, sizeof (Attribute));
	if (!attrs[2])
		goto err;

	oid = SECOID_FindOIDByTag(SEC_OID_PKCS9_SIGNING_TIME);
	attrs[2]->attrType = oid->oid;

	SECItem *signing_time[2] = { NULL, NULL };
	if (generate_time(cms, &encoded, time(NULL)) < 0)
		goto err;
	signing_time[0] = SECITEM_ArenaDupItem(cms->arena, &encoded);
	if (!signing_time[0])
		goto err;
	attrs[2]->attrValues = signing_time;

	/* build the fourth attribute, which is our PKCS9 message
	 * digest (which is a SHA-whatever selected and generated elsewhere */
	attrs[3] = PORT_ArenaZAlloc(cms->arena, sizeof (Attribute));
	if (!attrs[3])
		goto err;

	oid = SECOID_FindOIDByTag(SEC_OID_PKCS9_MESSAGE_DIGEST);
	attrs[3]->attrType = oid->oid;

	SECItem *digest_values[2] = { NULL, NULL };
	if (generate_octet_string(cms, &encoded, cms->ci_digest) < 0)
		goto err;
	digest_values[0] = SECITEM_ArenaDupItem(cms->arena, &encoded);
	if (!digest_values[0])
		goto err;
	attrs[3]->attrValues = digest_values;

	Attribute **attrtmp = attrs;
	if (SEC_ASN1EncodeItem(cms->arena, sattrs, &attrtmp,
				AttributeSetTemplate) == NULL)
		goto err;
	return 0;
err:
	return -1;
}