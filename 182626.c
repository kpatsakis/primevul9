gxps_fonts_new_ft_face (const gchar *font_uri,
			guchar      *font_data,
			gsize        font_data_len,
			FT_Face     *face)
{
	init_ft_lib ();

	if (FT_New_Memory_Face (ft_lib, font_data, font_data_len, 0, face)) {
		/* Failed to load, probably obfuscated font */
		gchar         *base_name;
		unsigned short guid[16];

		base_name = g_path_get_basename (font_uri);
		if (!parse_guid (base_name, guid)) {
			g_warning ("Failed to parse guid for font %s\n", font_uri);
			g_free (base_name);

			return FALSE;
		}
		g_free (base_name);

		if (font_data_len >= 32) {
			// Obfuscation - xor bytes in font binary with bytes from guid (font's filename)
			static const gint mapping[] = {15, 14, 13, 12, 11, 10, 9, 8, 6, 7, 4, 5, 0, 1, 2, 3};
			gint i;

			for (i = 0; i < 16; i++) {
				font_data[i] ^= guid[mapping[i]];
				font_data[i + 16] ^= guid[mapping[i]];
			}

			if (FT_New_Memory_Face (ft_lib, font_data, font_data_len, 0, face))
				return FALSE;
		} else {
			g_warning ("Font file is too small\n");
			return FALSE;
		}
	}

	return TRUE;
}