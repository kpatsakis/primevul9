__i915_gem_userptr_get_pages_worker(struct work_struct *_work)
{
	struct get_pages_work *work = container_of(_work, typeof(*work), work);
	struct drm_i915_gem_object *obj = work->obj;
	const unsigned long npages = obj->base.size >> PAGE_SHIFT;
	unsigned long pinned;
	struct page **pvec;
	int ret;

	ret = -ENOMEM;
	pinned = 0;

	pvec = kvmalloc_array(npages, sizeof(struct page *), GFP_KERNEL);
	if (pvec != NULL) {
		struct mm_struct *mm = obj->userptr.mm->mm;
		unsigned int flags = 0;
		int locked = 0;

		if (!i915_gem_object_is_readonly(obj))
			flags |= FOLL_WRITE;

		ret = -EFAULT;
		if (mmget_not_zero(mm)) {
			while (pinned < npages) {
				if (!locked) {
					down_read(&mm->mmap_sem);
					locked = 1;
				}
				ret = get_user_pages_remote
					(work->task, mm,
					 obj->userptr.ptr + pinned * PAGE_SIZE,
					 npages - pinned,
					 flags,
					 pvec + pinned, NULL, &locked);
				if (ret < 0)
					break;

				pinned += ret;
			}
			if (locked)
				up_read(&mm->mmap_sem);
			mmput(mm);
		}
	}

	mutex_lock_nested(&obj->mm.lock, I915_MM_GET_PAGES);
	if (obj->userptr.work == &work->work) {
		struct sg_table *pages = ERR_PTR(ret);

		if (pinned == npages) {
			pages = __i915_gem_userptr_alloc_pages(obj, pvec,
							       npages);
			if (!IS_ERR(pages)) {
				pinned = 0;
				pages = NULL;
			}
		}

		obj->userptr.work = ERR_CAST(pages);
		if (IS_ERR(pages))
			__i915_gem_userptr_set_active(obj, false);
	}
	mutex_unlock(&obj->mm.lock);

	release_pages(pvec, pinned);
	kvfree(pvec);

	i915_gem_object_put(obj);
	put_task_struct(work->task);
	kfree(work);
}