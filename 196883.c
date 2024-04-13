v3d_invalidate_l2c(struct v3d_dev *v3d, int core)
{
	if (v3d->ver > 32)
		return;

	V3D_CORE_WRITE(core, V3D_CTL_L2CACTL,
		       V3D_L2CACTL_L2CCLR |
		       V3D_L2CACTL_L2CENA);
}