v3d_flush_l2t(struct v3d_dev *v3d, int core)
{
	/* While there is a busy bit (V3D_L2TCACTL_L2TFLS), we don't
	 * need to wait for completion before dispatching the job --
	 * L2T accesses will be stalled until the flush has completed.
	 * However, we do need to make sure we don't try to trigger a
	 * new flush while the L2_CLEAN queue is trying to
	 * synchronously clean after a job.
	 */
	mutex_lock(&v3d->cache_clean_lock);
	V3D_CORE_WRITE(core, V3D_CTL_L2TCACTL,
		       V3D_L2TCACTL_L2TFLS |
		       V3D_SET_FIELD(V3D_L2TCACTL_FLM_FLUSH, V3D_L2TCACTL_FLM));
	mutex_unlock(&v3d->cache_clean_lock);
}