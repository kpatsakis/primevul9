v3d_gem_destroy(struct drm_device *dev)
{
	struct v3d_dev *v3d = to_v3d_dev(dev);

	v3d_sched_fini(v3d);

	/* Waiting for jobs to finish would need to be done before
	 * unregistering V3D.
	 */
	WARN_ON(v3d->bin_job);
	WARN_ON(v3d->render_job);

	drm_mm_takedown(&v3d->mm);

	dma_free_coherent(v3d->dev, 4096 * 1024, (void *)v3d->pt, v3d->pt_paddr);
}