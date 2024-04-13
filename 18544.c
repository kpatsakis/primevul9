generate_empty_sequence(cms_context *cms, SECItem *encoded)
{
	SECItem empty = {.type = SEC_ASN1_SEQUENCE,
			 .data = NULL,
			 .len = 0
	};
	void *ret;
	ret = SEC_ASN1EncodeItem(cms->arena, encoded, &empty,
							EmptySequenceTemplate);
	if (ret == NULL)
		cmsreterr(-1, cms, "could not encode empty sequence");
	return 0;
}