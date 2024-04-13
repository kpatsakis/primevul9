make_context_specific(cms_context *cms, int ctxt, SECItem *encoded,
			SECItem *original)
{
	void *rv;
	ContextSpecificSequence[0].kind = SEC_ASN1_EXPLICIT |
					  SEC_ASN1_CONTEXT_SPECIFIC | ctxt;

	rv = SEC_ASN1EncodeItem(cms->arena, encoded, original,
				ContextSpecificSequence);
	if (rv == NULL)
		cmsreterr(-1, cms, "could not encode context specific data");
	return 0;
}