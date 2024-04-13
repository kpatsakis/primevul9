pdf_set_cmap_wmode(fz_context *ctx, pdf_cmap *cmap, int wmode)
{
	cmap->wmode = wmode;
}