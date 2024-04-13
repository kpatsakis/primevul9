pdf_keep_cmap(fz_context *ctx, pdf_cmap *cmap)
{
	return fz_keep_storable(ctx, &cmap->storable);
}