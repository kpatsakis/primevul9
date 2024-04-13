generate_octet_string(cms_context *cms, SECItem *encoded, SECItem *original)
{
	if (content_is_empty(original->data, original->len)) {
		cms->log(cms, LOG_ERR, "content is empty, not encoding");
		return -1;
	}
	if (SEC_ASN1EncodeItem(cms->arena, encoded, original,
			SEC_OctetStringTemplate) == NULL)
		cmsreterr(-1, cms, "could not encode octet string");

	return 0;
}