GF_Descriptor *gf_odf_new_tx3g()
{
	GF_TextSampleDescriptor *newDesc = (GF_TextSampleDescriptor*) gf_malloc(sizeof(GF_TextSampleDescriptor));
	if (!newDesc) return NULL;
	memset(newDesc, 0, sizeof(GF_TextSampleDescriptor));
	newDesc->tag = GF_ODF_TX3G_TAG;
	return (GF_Descriptor *) newDesc;
}