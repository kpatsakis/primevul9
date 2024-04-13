pdf_map_range_to_range(fz_context *ctx, pdf_cmap *cmap, unsigned int low, unsigned int high, int out)
{
	add_range(ctx, cmap, low, high, out, 1, 0);
}