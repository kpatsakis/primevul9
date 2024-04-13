generate_unsigned_attributes(cms_context *cms, SECItem *uattrs)
{
	Attribute *attrs[1];
	memset(attrs, '\0', sizeof (attrs));

	Attribute **attrtmp = attrs;
	if (SEC_ASN1EncodeItem(cms->arena, uattrs, &attrtmp,
				AttributeSetTemplate) == NULL)
		goto err;
	return 0;
err:
	return -1;
}