wrap_in_seq(cms_context *cms, SECItem *der, SECItem *items, int num_items)
{
	void *ret;

	void *mark = PORT_ArenaMark(cms->arena);

	SEC_ASN1Template tmpl[num_items+2];

	memcpy(&tmpl[0], &SeqTemplateHeader, sizeof(*tmpl));
	tmpl[0].size = sizeof (SECItem) * num_items;

	for (int i = 0; i < num_items; i++) {
		memcpy(&tmpl[i+1], &SeqTemplateTemplate, sizeof(SEC_ASN1Template));
		tmpl[i+1].offset = (i) * sizeof (SECItem);
	}
	memset(&tmpl[num_items + 1], '\0', sizeof(SEC_ASN1Template));

	int rc = 0;
	ret = SEC_ASN1EncodeItem(cms->arena, der, items, tmpl);
	if (ret == NULL) {
		save_port_err() {
			PORT_ArenaRelease(cms->arena, mark);
		}
		cmsreterr(-1, cms, "could not encode set");
	}
	PORT_ArenaUnmark(cms->arena, mark);
	return rc;
}