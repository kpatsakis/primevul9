ft_font_face_free (FtFontFace *font_face)
{
	if (!font_face)
		return;

	g_slice_free (FtFontFace, font_face);
}