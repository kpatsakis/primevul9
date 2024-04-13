GF_Err gf_odf_del_tx3g(GF_TextSampleDescriptor *sd)
{
	u32 i;
	for (i=0; i<sd->font_count; i++)
		if (sd->fonts[i].fontName) gf_free(sd->fonts[i].fontName);
	gf_free(sd->fonts);
	gf_free(sd);
	return GF_OK;
}