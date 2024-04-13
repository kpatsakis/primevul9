v3d_push_job(struct v3d_file_priv *v3d_priv,
	     struct v3d_job *job, enum v3d_queue queue)
{
	int ret;

	ret = drm_sched_job_init(&job->base, &v3d_priv->sched_entity[queue],
				 v3d_priv);
	if (ret)
		return ret;

	job->done_fence = dma_fence_get(&job->base.s_fence->finished);

	/* put by scheduler job completion */
	kref_get(&job->refcount);

	drm_sched_entity_push_job(&job->base, &v3d_priv->sched_entity[queue]);

	return 0;
}