xps_identify_font_encoding(fz_font *font, int idx, int *pid, int *eid)
{
	FT_Face face = font->ft_face;
	*pid = face->charmaps[idx]->platform_id;
	*eid = face->charmaps[idx]->encoding_id;
}