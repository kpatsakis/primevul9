xps_select_font_encoding(fz_font *font, int idx)
{
	FT_Face face = font->ft_face;
	FT_Set_Charmap(face, face->charmaps[idx]);
}