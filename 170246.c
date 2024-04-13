pdf_map_one_to_many(fz_context *ctx, pdf_cmap *cmap, unsigned int low, int *values, int len)
{
	if (len == 1)
	{
		add_range(ctx, cmap, low, low, values[0], 1, 0);
		return;
	}

	/* Decode unicode surrogate pairs. */
	/* Only the *-UCS2 CMaps use one-to-many mappings, so assuming unicode should be safe. */
	if (len == 2 &&
		values[0] >= 0xD800 && values[0] <= 0xDBFF &&
		values[1] >= 0xDC00 && values[1] <= 0xDFFF)
	{
		int rune = ((values[0] - 0xD800) << 10) + (values[1] - 0xDC00) + 0x10000;
		add_range(ctx, cmap, low, low, rune, 1, 0);
		return;
	}

	if (len > PDF_MRANGE_CAP)
	{
		fz_warn(ctx, "ignoring one to many mapping in cmap %s", cmap->cmap_name);
		return;
	}

	add_mrange(ctx, cmap, low, values, len);
}