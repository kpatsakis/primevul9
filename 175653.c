static int intel_vgpu_register_reg(struct intel_vgpu *vgpu,
		unsigned int type, unsigned int subtype,
		const struct intel_vgpu_regops *ops,
		size_t size, u32 flags, void *data)
{
	struct vfio_region *region;

	region = krealloc(vgpu->vdev.region,
			(vgpu->vdev.num_regions + 1) * sizeof(*region),
			GFP_KERNEL);
	if (!region)
		return -ENOMEM;

	vgpu->vdev.region = region;
	vgpu->vdev.region[vgpu->vdev.num_regions].type = type;
	vgpu->vdev.region[vgpu->vdev.num_regions].subtype = subtype;
	vgpu->vdev.region[vgpu->vdev.num_regions].ops = ops;
	vgpu->vdev.region[vgpu->vdev.num_regions].size = size;
	vgpu->vdev.region[vgpu->vdev.num_regions].flags = flags;
	vgpu->vdev.region[vgpu->vdev.num_regions].data = data;
	vgpu->vdev.num_regions++;
	return 0;
}