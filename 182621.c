get_ft_font_face_cache (void)
{
	if (g_once_init_enter (&ft_font_face_cache)) {
		GHashTable *h;

		h = g_hash_table_new_full (ft_font_face_hash,
					   ft_font_face_equal,
					   (GDestroyNotify)ft_font_face_free,
					   (GDestroyNotify)cairo_font_face_destroy);
		g_once_init_leave (&ft_font_face_cache, (gsize)h);
	}

	return (GHashTable *)ft_font_face_cache;
}