pdf_drop_cmap(fz_context *ctx, pdf_cmap *cmap)
{
	fz_drop_storable(ctx, &cmap->storable);
}