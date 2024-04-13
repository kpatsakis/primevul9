__i915_gem_userptr_alloc_pages(struct drm_i915_gem_object *obj,
			       struct page **pvec, unsigned long num_pages)
{
	unsigned int max_segment = i915_sg_segment_size();
	struct sg_table *st;
	unsigned int sg_page_sizes;
	int ret;

	st = kmalloc(sizeof(*st), GFP_KERNEL);
	if (!st)
		return ERR_PTR(-ENOMEM);

alloc_table:
	ret = __sg_alloc_table_from_pages(st, pvec, num_pages,
					  0, num_pages << PAGE_SHIFT,
					  max_segment,
					  GFP_KERNEL);
	if (ret) {
		kfree(st);
		return ERR_PTR(ret);
	}

	ret = i915_gem_gtt_prepare_pages(obj, st);
	if (ret) {
		sg_free_table(st);

		if (max_segment > PAGE_SIZE) {
			max_segment = PAGE_SIZE;
			goto alloc_table;
		}

		kfree(st);
		return ERR_PTR(ret);
	}

	sg_page_sizes = i915_sg_page_sizes(st->sgl);

	__i915_gem_object_set_pages(obj, st, sg_page_sizes);

	return st;
}