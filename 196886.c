v3d_job_free(struct kref *ref)
{
	struct v3d_job *job = container_of(ref, struct v3d_job, refcount);
	unsigned long index;
	struct dma_fence *fence;
	int i;

	for (i = 0; i < job->bo_count; i++) {
		if (job->bo[i])
			drm_gem_object_put_unlocked(job->bo[i]);
	}
	kvfree(job->bo);

	xa_for_each(&job->deps, index, fence) {
		dma_fence_put(fence);
	}
	xa_destroy(&job->deps);

	dma_fence_put(job->irq_fence);
	dma_fence_put(job->done_fence);

	pm_runtime_mark_last_busy(job->v3d->dev);
	pm_runtime_put_autosuspend(job->v3d->dev);

	kfree(job);
}