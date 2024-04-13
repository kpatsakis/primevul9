void ResetTextConfig(GF_TextConfig *desc)
{
	GF_List *bck;
	while (gf_list_count(desc->sample_descriptions)) {
		GF_TextSampleDescriptor *sd = (GF_TextSampleDescriptor *)gf_list_get(desc->sample_descriptions, 0);
		gf_list_rem(desc->sample_descriptions, 0);
		gf_odf_del_tx3g(sd);
	}
	bck = desc->sample_descriptions;
	memset(desc, 0, sizeof(GF_TextConfig));
	desc->tag = GF_ODF_TEXT_CFG_TAG;
	desc->sample_descriptions = bck;
}