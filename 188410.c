xps_insert_font(xps_document *doc, char *name, fz_font *font)
{
	xps_font_cache *cache = fz_malloc_struct(doc->ctx, xps_font_cache);
	cache->name = fz_strdup(doc->ctx, name);
	cache->font = fz_keep_font(doc->ctx, font);
	cache->next = doc->font_table;
	doc->font_table = cache;
}