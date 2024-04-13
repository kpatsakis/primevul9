GF_Descriptor *gf_odf_new_text_cfg()
{
	GF_TextConfig *newDesc = (GF_TextConfig*) gf_malloc(sizeof(GF_TextConfig));
	if (!newDesc) return NULL;
	memset(newDesc, 0, sizeof(GF_TextConfig));
	newDesc->tag = GF_ODF_TEXT_CFG_TAG;
	newDesc->sample_descriptions = gf_list_new();
	newDesc->Base3GPPFormat = 0x10;
	newDesc->MPEGExtendedFormat = 0x10;
	newDesc->profileLevel = 0x10;
	newDesc->timescale = 1000;
	return (GF_Descriptor *) newDesc;
}