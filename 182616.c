ft_font_face_equal (gconstpointer v1,
		    gconstpointer v2)
{
	FtFontFace *ft_face_1 = (FtFontFace *)v1;
	FtFontFace *ft_face_2 = (FtFontFace *)v2;

	if (ft_face_1->font_data_len != ft_face_2->font_data_len)
		return FALSE;

	return memcmp (ft_face_1->font_data, ft_face_2->font_data, ft_face_1->font_data_len) == 0;
}