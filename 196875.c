v3d_submit_csd_ioctl(struct drm_device *dev, void *data,
		     struct drm_file *file_priv)
{
	struct v3d_dev *v3d = to_v3d_dev(dev);
	struct v3d_file_priv *v3d_priv = file_priv->driver_priv;
	struct drm_v3d_submit_csd *args = data;
	struct v3d_csd_job *job;
	struct v3d_job *clean_job;
	struct ww_acquire_ctx acquire_ctx;
	int ret;

	trace_v3d_submit_csd_ioctl(&v3d->drm, args->cfg[5], args->cfg[6]);

	if (!v3d_has_csd(v3d)) {
		DRM_DEBUG("Attempting CSD submit on non-CSD hardware\n");
		return -EINVAL;
	}

	job = kcalloc(1, sizeof(*job), GFP_KERNEL);
	if (!job)
		return -ENOMEM;

	ret = v3d_job_init(v3d, file_priv, &job->base,
			   v3d_job_free, args->in_sync);
	if (ret) {
		kfree(job);
		return ret;
	}

	clean_job = kcalloc(1, sizeof(*clean_job), GFP_KERNEL);
	if (!clean_job) {
		v3d_job_put(&job->base);
		kfree(job);
		return -ENOMEM;
	}

	ret = v3d_job_init(v3d, file_priv, clean_job, v3d_job_free, 0);
	if (ret) {
		v3d_job_put(&job->base);
		kfree(clean_job);
		return ret;
	}

	job->args = *args;

	ret = v3d_lookup_bos(dev, file_priv, clean_job,
			     args->bo_handles, args->bo_handle_count);
	if (ret)
		goto fail;

	ret = v3d_lock_bo_reservations(clean_job, &acquire_ctx);
	if (ret)
		goto fail;

	mutex_lock(&v3d->sched_lock);
	ret = v3d_push_job(v3d_priv, &job->base, V3D_CSD);
	if (ret)
		goto fail_unreserve;

	ret = drm_gem_fence_array_add(&clean_job->deps,
				      dma_fence_get(job->base.done_fence));
	if (ret)
		goto fail_unreserve;

	ret = v3d_push_job(v3d_priv, clean_job, V3D_CACHE_CLEAN);
	if (ret)
		goto fail_unreserve;
	mutex_unlock(&v3d->sched_lock);

	v3d_attach_fences_and_unlock_reservation(file_priv,
						 clean_job,
						 &acquire_ctx,
						 args->out_sync,
						 clean_job->done_fence);

	v3d_job_put(&job->base);
	v3d_job_put(clean_job);

	return 0;

fail_unreserve:
	mutex_unlock(&v3d->sched_lock);
	drm_gem_unlock_reservations(clean_job->bo, clean_job->bo_count,
				    &acquire_ctx);
fail:
	v3d_job_put(&job->base);
	v3d_job_put(clean_job);

	return ret;
}