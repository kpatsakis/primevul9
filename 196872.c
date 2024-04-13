v3d_job_init(struct v3d_dev *v3d, struct drm_file *file_priv,
	     struct v3d_job *job, void (*free)(struct kref *ref),
	     u32 in_sync)
{
	struct dma_fence *in_fence = NULL;
	int ret;

	job->v3d = v3d;
	job->free = free;

	ret = pm_runtime_get_sync(v3d->dev);
	if (ret < 0)
		return ret;

	xa_init_flags(&job->deps, XA_FLAGS_ALLOC);

	ret = drm_syncobj_find_fence(file_priv, in_sync, 0, 0, &in_fence);
	if (ret == -EINVAL)
		goto fail;

	ret = drm_gem_fence_array_add(&job->deps, in_fence);
	if (ret)
		goto fail;

	kref_init(&job->refcount);

	return 0;
fail:
	xa_destroy(&job->deps);
	pm_runtime_put_autosuspend(v3d->dev);
	return ret;
}