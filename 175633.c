static int kvmgt_get_vfio_device(void *p_vgpu)
{
	struct intel_vgpu *vgpu = (struct intel_vgpu *)p_vgpu;

	vgpu->vdev.vfio_device = vfio_device_get_from_dev(
		mdev_dev(vgpu->vdev.mdev));
	if (!vgpu->vdev.vfio_device) {
		gvt_vgpu_err("failed to get vfio device\n");
		return -ENODEV;
	}
	return 0;
}