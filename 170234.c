pdf_drop_cmap_imp(fz_context *ctx, fz_storable *cmap_)
{
	pdf_cmap *cmap = (pdf_cmap *)cmap_;
	pdf_drop_cmap(ctx, cmap->usecmap);
	fz_free(ctx, cmap->ranges);
	fz_free(ctx, cmap->xranges);
	fz_free(ctx, cmap->mranges);
	fz_free(ctx, cmap->dict);
	fz_free(ctx, cmap->tree);
	fz_free(ctx, cmap);
}