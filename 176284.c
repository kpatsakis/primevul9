__i915_gem_userptr_get_pages_schedule(struct drm_i915_gem_object *obj)
{
	struct get_pages_work *work;

	/* Spawn a worker so that we can acquire the
	 * user pages without holding our mutex. Access
	 * to the user pages requires mmap_sem, and we have
	 * a strict lock ordering of mmap_sem, struct_mutex -
	 * we already hold struct_mutex here and so cannot
	 * call gup without encountering a lock inversion.
	 *
	 * Userspace will keep on repeating the operation
	 * (thanks to EAGAIN) until either we hit the fast
	 * path or the worker completes. If the worker is
	 * cancelled or superseded, the task is still run
	 * but the results ignored. (This leads to
	 * complications that we may have a stray object
	 * refcount that we need to be wary of when
	 * checking for existing objects during creation.)
	 * If the worker encounters an error, it reports
	 * that error back to this function through
	 * obj->userptr.work = ERR_PTR.
	 */
	work = kmalloc(sizeof(*work), GFP_KERNEL);
	if (work == NULL)
		return ERR_PTR(-ENOMEM);

	obj->userptr.work = &work->work;

	work->obj = i915_gem_object_get(obj);

	work->task = current;
	get_task_struct(work->task);

	INIT_WORK(&work->work, __i915_gem_userptr_get_pages_worker);
	queue_work(to_i915(obj->base.dev)->mm.userptr_wq, &work->work);

	return ERR_PTR(-EAGAIN);
}