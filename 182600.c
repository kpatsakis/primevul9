ft_font_face_new (guchar *font_data,
		  gsize   font_data_len)
{
	FtFontFace *ff;

	ff = g_slice_new (FtFontFace);

	ff->font_data = font_data;
	ff->font_data_len = font_data_len;

	return ff;
}