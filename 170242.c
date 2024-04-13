add_mrange(fz_context *ctx, pdf_cmap *cmap, unsigned int low, int *out, int len)
{
	int out_pos;

	if (cmap->dlen + len + 1 > cmap->dcap)
	{
		int new_cap = cmap->dcap ? cmap->dcap * 2 : 256;
		cmap->dict = fz_resize_array(ctx, cmap->dict, new_cap, sizeof *cmap->dict);
		cmap->dcap = new_cap;
	}
	out_pos = cmap->dlen;
	cmap->dict[out_pos] = len;
	memcpy(&cmap->dict[out_pos+1], out, sizeof(int)*len);
	cmap->dlen += len + 1;

	add_range(ctx, cmap, low, low, out_pos, 1, 1);
}