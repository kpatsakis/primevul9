static struct gvt_dma *__gvt_cache_find_gfn(struct intel_vgpu *vgpu, gfn_t gfn)
{
	struct rb_node *node = vgpu->vdev.gfn_cache.rb_node;
	struct gvt_dma *itr;

	while (node) {
		itr = rb_entry(node, struct gvt_dma, gfn_node);

		if (gfn < itr->gfn)
			node = node->rb_left;
		else if (gfn > itr->gfn)
			node = node->rb_right;
		else
			return itr;
	}
	return NULL;
}