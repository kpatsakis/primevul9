copy_node_types(cmap_splay *node, void *arg)
{
	pdf_cmap *cmap = (pdf_cmap *)arg;

	if (node->many)
	{
		assert(node->low == node->high);
		cmap->mranges[cmap->mlen].low = node->low;
		cmap->mranges[cmap->mlen].out = node->out;
		cmap->mlen++;
	}
	else if (node->low <= 0xffff && node->high <= 0xFFFF && node->out <= 0xFFFF)
	{
		cmap->ranges[cmap->rlen].low = node->low;
		cmap->ranges[cmap->rlen].high = node->high;
		cmap->ranges[cmap->rlen].out = node->out;
		cmap->rlen++;
	}
	else
	{
		cmap->xranges[cmap->xlen].low = node->low;
		cmap->xranges[cmap->xlen].high = node->high;
		cmap->xranges[cmap->xlen].out = node->out;
		cmap->xlen++;
	}
}