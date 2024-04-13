v3d_clean_caches(struct v3d_dev *v3d)
{
	struct drm_device *dev = &v3d->drm;
	int core = 0;

	trace_v3d_cache_clean_begin(dev);

	V3D_CORE_WRITE(core, V3D_CTL_L2TCACTL, V3D_L2TCACTL_TMUWCF);
	if (wait_for(!(V3D_CORE_READ(core, V3D_CTL_L2TCACTL) &
		       V3D_L2TCACTL_L2TFLS), 100)) {
		DRM_ERROR("Timeout waiting for L1T write combiner flush\n");
	}

	mutex_lock(&v3d->cache_clean_lock);
	V3D_CORE_WRITE(core, V3D_CTL_L2TCACTL,
		       V3D_L2TCACTL_L2TFLS |
		       V3D_SET_FIELD(V3D_L2TCACTL_FLM_CLEAN, V3D_L2TCACTL_FLM));

	if (wait_for(!(V3D_CORE_READ(core, V3D_CTL_L2TCACTL) &
		       V3D_L2TCACTL_L2TFLS), 100)) {
		DRM_ERROR("Timeout waiting for L2T clean\n");
	}

	mutex_unlock(&v3d->cache_clean_lock);

	trace_v3d_cache_clean_end(dev);
}