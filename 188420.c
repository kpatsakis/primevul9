xps_encode_font_char(fz_font *font, int code)
{
	FT_Face face = font->ft_face;
	int gid = FT_Get_Char_Index(face, code);
	if (gid == 0 && face->charmap && face->charmap->platform_id == 3 && face->charmap->encoding_id == 0)
		gid = FT_Get_Char_Index(face, 0xF000 | code);
	return gid;
}