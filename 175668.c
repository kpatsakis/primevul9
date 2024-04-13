static int __gvt_cache_add(struct intel_vgpu *vgpu, gfn_t gfn,
		dma_addr_t dma_addr, unsigned long size)
{
	struct gvt_dma *new, *itr;
	struct rb_node **link, *parent = NULL;

	new = kzalloc(sizeof(struct gvt_dma), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	new->vgpu = vgpu;
	new->gfn = gfn;
	new->dma_addr = dma_addr;
	new->size = size;
	kref_init(&new->ref);

	/* gfn_cache maps gfn to struct gvt_dma. */
	link = &vgpu->vdev.gfn_cache.rb_node;
	while (*link) {
		parent = *link;
		itr = rb_entry(parent, struct gvt_dma, gfn_node);

		if (gfn < itr->gfn)
			link = &parent->rb_left;
		else
			link = &parent->rb_right;
	}
	rb_link_node(&new->gfn_node, parent, link);
	rb_insert_color(&new->gfn_node, &vgpu->vdev.gfn_cache);

	/* dma_addr_cache maps dma addr to struct gvt_dma. */
	parent = NULL;
	link = &vgpu->vdev.dma_addr_cache.rb_node;
	while (*link) {
		parent = *link;
		itr = rb_entry(parent, struct gvt_dma, dma_addr_node);

		if (dma_addr < itr->dma_addr)
			link = &parent->rb_left;
		else
			link = &parent->rb_right;
	}
	rb_link_node(&new->dma_addr_node, parent, link);
	rb_insert_color(&new->dma_addr_node, &vgpu->vdev.dma_addr_cache);

	vgpu->vdev.nr_cache_entries++;
	return 0;
}