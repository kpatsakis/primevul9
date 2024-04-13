pdf_sort_cmap(fz_context *ctx, pdf_cmap *cmap)
{
	int counts[3];

	if (cmap->tree == NULL)
		return;

	counts[0] = 0;
	counts[1] = 0;
	counts[2] = 0;
	walk_splay(cmap->tree, cmap->ttop, count_node_types, &counts);

	cmap->ranges = fz_malloc_array(ctx, counts[0], sizeof(*cmap->ranges));
	cmap->rcap = counts[0];
	cmap->xranges = fz_malloc_array(ctx, counts[1], sizeof(*cmap->xranges));
	cmap->xcap = counts[1];
	cmap->mranges = fz_malloc_array(ctx, counts[2], sizeof(*cmap->mranges));
	cmap->mcap = counts[2];

	walk_splay(cmap->tree, cmap->ttop, copy_node_types, cmap);

	fz_free(ctx, cmap->tree);
	cmap->tree = NULL;
}