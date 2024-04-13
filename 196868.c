v3d_reset_v3d(struct v3d_dev *v3d)
{
	if (v3d->reset)
		reset_control_reset(v3d->reset);
	else
		v3d_reset_by_bridge(v3d);

	v3d_init_hw_state(v3d);
}