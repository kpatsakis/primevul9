i915_gem_userptr_put_pages(struct drm_i915_gem_object *obj,
			   struct sg_table *pages)
{
	struct sgt_iter sgt_iter;
	struct page *page;

	/* Cancel any inflight work and force them to restart their gup */
	obj->userptr.work = NULL;
	__i915_gem_userptr_set_active(obj, false);
	if (!pages)
		return;

	__i915_gem_object_release_shmem(obj, pages, true);
	i915_gem_gtt_finish_pages(obj, pages);

	/*
	 * We always mark objects as dirty when they are used by the GPU,
	 * just in case. However, if we set the vma as being read-only we know
	 * that the object will never have been written to.
	 */
	if (i915_gem_object_is_readonly(obj))
		obj->mm.dirty = false;

	for_each_sgt_page(page, sgt_iter, pages) {
		if (obj->mm.dirty && trylock_page(page)) {
			/*
			 * As this may not be anonymous memory (e.g. shmem)
			 * but exist on a real mapping, we have to lock
			 * the page in order to dirty it -- holding
			 * the page reference is not sufficient to
			 * prevent the inode from being truncated.
			 * Play safe and take the lock.
			 *
			 * However...!
			 *
			 * The mmu-notifier can be invalidated for a
			 * migrate_page, that is alreadying holding the lock
			 * on the page. Such a try_to_unmap() will result
			 * in us calling put_pages() and so recursively try
			 * to lock the page. We avoid that deadlock with
			 * a trylock_page() and in exchange we risk missing
			 * some page dirtying.
			 */
			set_page_dirty(page);
			unlock_page(page);
		}

		mark_page_accessed(page);
		put_page(page);
	}
	obj->mm.dirty = false;

	sg_free_table(pages);
	kfree(pages);
}