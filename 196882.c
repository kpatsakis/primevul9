v3d_attach_fences_and_unlock_reservation(struct drm_file *file_priv,
					 struct v3d_job *job,
					 struct ww_acquire_ctx *acquire_ctx,
					 u32 out_sync,
					 struct dma_fence *done_fence)
{
	struct drm_syncobj *sync_out;
	int i;

	for (i = 0; i < job->bo_count; i++) {
		/* XXX: Use shared fences for read-only objects. */
		dma_resv_add_excl_fence(job->bo[i]->resv,
						  job->done_fence);
	}

	drm_gem_unlock_reservations(job->bo, job->bo_count, acquire_ctx);

	/* Update the return sync object for the job */
	sync_out = drm_syncobj_find(file_priv, out_sync);
	if (sync_out) {
		drm_syncobj_replace_fence(sync_out, done_fence);
		drm_syncobj_put(sync_out);
	}
}