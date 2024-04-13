v3d_reset(struct v3d_dev *v3d)
{
	struct drm_device *dev = &v3d->drm;

	DRM_DEV_ERROR(dev->dev, "Resetting GPU for hang.\n");
	DRM_DEV_ERROR(dev->dev, "V3D_ERR_STAT: 0x%08x\n",
		      V3D_CORE_READ(0, V3D_ERR_STAT));
	trace_v3d_reset_begin(dev);

	/* XXX: only needed for safe powerdown, not reset. */
	if (false)
		v3d_idle_axi(v3d, 0);

	v3d_idle_gca(v3d);
	v3d_reset_v3d(v3d);

	v3d_mmu_set_page_table(v3d);
	v3d_irq_reset(v3d);

	trace_v3d_reset_end(dev);
}