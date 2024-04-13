pdf_new_cmap(fz_context *ctx)
{
	pdf_cmap *cmap = fz_malloc_struct(ctx, pdf_cmap);
	FZ_INIT_STORABLE(cmap, 1, pdf_drop_cmap_imp);
	return cmap;
}