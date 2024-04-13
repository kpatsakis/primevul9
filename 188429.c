xps_lookup_font(xps_document *doc, char *name)
{
	xps_font_cache *cache;
	for (cache = doc->font_table; cache; cache = cache->next)
		if (!xps_strcasecmp(cache->name, name))
			return fz_keep_font(doc->ctx, cache->font);
	return NULL;
}