v3d_submit_tfu_ioctl(struct drm_device *dev, void *data,
		     struct drm_file *file_priv)
{
	struct v3d_dev *v3d = to_v3d_dev(dev);
	struct v3d_file_priv *v3d_priv = file_priv->driver_priv;
	struct drm_v3d_submit_tfu *args = data;
	struct v3d_tfu_job *job;
	struct ww_acquire_ctx acquire_ctx;
	int ret = 0;

	trace_v3d_submit_tfu_ioctl(&v3d->drm, args->iia);

	job = kcalloc(1, sizeof(*job), GFP_KERNEL);
	if (!job)
		return -ENOMEM;

	ret = v3d_job_init(v3d, file_priv, &job->base,
			   v3d_job_free, args->in_sync);
	if (ret) {
		kfree(job);
		return ret;
	}

	job->base.bo = kcalloc(ARRAY_SIZE(args->bo_handles),
			       sizeof(*job->base.bo), GFP_KERNEL);
	if (!job->base.bo) {
		v3d_job_put(&job->base);
		return -ENOMEM;
	}

	job->args = *args;

	spin_lock(&file_priv->table_lock);
	for (job->base.bo_count = 0;
	     job->base.bo_count < ARRAY_SIZE(args->bo_handles);
	     job->base.bo_count++) {
		struct drm_gem_object *bo;

		if (!args->bo_handles[job->base.bo_count])
			break;

		bo = idr_find(&file_priv->object_idr,
			      args->bo_handles[job->base.bo_count]);
		if (!bo) {
			DRM_DEBUG("Failed to look up GEM BO %d: %d\n",
				  job->base.bo_count,
				  args->bo_handles[job->base.bo_count]);
			ret = -ENOENT;
			spin_unlock(&file_priv->table_lock);
			goto fail;
		}
		drm_gem_object_get(bo);
		job->base.bo[job->base.bo_count] = bo;
	}
	spin_unlock(&file_priv->table_lock);

	ret = v3d_lock_bo_reservations(&job->base, &acquire_ctx);
	if (ret)
		goto fail;

	mutex_lock(&v3d->sched_lock);
	ret = v3d_push_job(v3d_priv, &job->base, V3D_TFU);
	if (ret)
		goto fail_unreserve;
	mutex_unlock(&v3d->sched_lock);

	v3d_attach_fences_and_unlock_reservation(file_priv,
						 &job->base, &acquire_ctx,
						 args->out_sync,
						 job->base.done_fence);

	v3d_job_put(&job->base);

	return 0;

fail_unreserve:
	mutex_unlock(&v3d->sched_lock);
	drm_gem_unlock_reservations(job->base.bo, job->base.bo_count,
				    &acquire_ctx);
fail:
	v3d_job_put(&job->base);

	return ret;
}