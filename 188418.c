xps_count_font_encodings(fz_font *font)
{
	FT_Face face = font->ft_face;
	return face->num_charmaps;
}