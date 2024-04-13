v3d_render_job_free(struct kref *ref)
{
	struct v3d_render_job *job = container_of(ref, struct v3d_render_job,
						  base.refcount);
	struct v3d_bo *bo, *save;

	list_for_each_entry_safe(bo, save, &job->unref_list, unref_head) {
		drm_gem_object_put_unlocked(&bo->base.base);
	}

	v3d_job_free(ref);
}