gxps_fonts_get_font (GXPSArchive *zip,
		     const gchar *font_uri,
		     GError     **error)
{
	GHashTable        *fonts_cache;
	cairo_font_face_t *font_face = NULL;

	fonts_cache = g_object_get_data (G_OBJECT (zip), FONTS_CACHE_KEY);
	if (fonts_cache) {
		font_face = g_hash_table_lookup (fonts_cache, font_uri);
		if (font_face)
			return font_face;
	}

	font_face = gxps_fonts_new_font_face (zip, font_uri, error);
	if (font_face) {
		if (!fonts_cache) {
			fonts_cache = g_hash_table_new_full (g_str_hash,
							     g_str_equal,
							     (GDestroyNotify)g_free,
							     (GDestroyNotify)cairo_font_face_destroy);
			g_object_set_data_full (G_OBJECT (zip), FONTS_CACHE_KEY,
						fonts_cache,
						(GDestroyNotify)g_hash_table_destroy);
		}

		g_hash_table_insert (fonts_cache,
				     g_strdup (font_uri),
				     cairo_font_face_reference (font_face));
	}

	return font_face;
}