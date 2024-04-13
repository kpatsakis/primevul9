pdf_add_codespace(fz_context *ctx, pdf_cmap *cmap, unsigned int low, unsigned int high, int n)
{
	if (cmap->codespace_len + 1 == nelem(cmap->codespace))
	{
		fz_warn(ctx, "assert: too many code space ranges");
		return;
	}

	cmap->codespace[cmap->codespace_len].n = n;
	cmap->codespace[cmap->codespace_len].low = low;
	cmap->codespace[cmap->codespace_len].high = high;
	cmap->codespace_len ++;
}