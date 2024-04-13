ft_font_face_hash (gconstpointer v)
{
	FtFontFace *ft_face = (FtFontFace *)v;
	guchar     *bytes = ft_face->font_data;
	gssize      len = ft_face->font_data_len;
	guint       hash = 5381;

	while (len--) {
		guchar c = *bytes++;

		hash *= 33;
		hash ^= c;
	}

	return hash;
}