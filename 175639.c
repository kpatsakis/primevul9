static void kvmgt_put_vfio_device(void *vgpu)
{
	if (WARN_ON(!((struct intel_vgpu *)vgpu)->vdev.vfio_device))
		return;

	vfio_device_put(((struct intel_vgpu *)vgpu)->vdev.vfio_device);
}