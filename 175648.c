static void kvmgt_detach_vgpu(void *p_vgpu)
{
	int i;
	struct intel_vgpu *vgpu = (struct intel_vgpu *)p_vgpu;

	if (!vgpu->vdev.region)
		return;

	for (i = 0; i < vgpu->vdev.num_regions; i++)
		if (vgpu->vdev.region[i].ops->release)
			vgpu->vdev.region[i].ops->release(vgpu,
					&vgpu->vdev.region[i]);
	vgpu->vdev.num_regions = 0;
	kfree(vgpu->vdev.region);
	vgpu->vdev.region = NULL;
}