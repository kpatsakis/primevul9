v3d_lock_bo_reservations(struct v3d_job *job,
			 struct ww_acquire_ctx *acquire_ctx)
{
	int i, ret;

	ret = drm_gem_lock_reservations(job->bo, job->bo_count, acquire_ctx);
	if (ret)
		return ret;

	for (i = 0; i < job->bo_count; i++) {
		ret = drm_gem_fence_array_add_implicit(&job->deps,
						       job->bo[i], true);
		if (ret) {
			drm_gem_unlock_reservations(job->bo, job->bo_count,
						    acquire_ctx);
			return ret;
		}
	}

	return 0;
}