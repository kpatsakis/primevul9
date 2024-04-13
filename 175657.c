static size_t intel_vgpu_reg_rw_opregion(struct intel_vgpu *vgpu, char *buf,
		size_t count, loff_t *ppos, bool iswrite)
{
	unsigned int i = VFIO_PCI_OFFSET_TO_INDEX(*ppos) -
			VFIO_PCI_NUM_REGIONS;
	void *base = vgpu->vdev.region[i].data;
	loff_t pos = *ppos & VFIO_PCI_OFFSET_MASK;

	if (pos >= vgpu->vdev.region[i].size || iswrite) {
		gvt_vgpu_err("invalid op or offset for Intel vgpu OpRegion\n");
		return -EINVAL;
	}
	count = min(count, (size_t)(vgpu->vdev.region[i].size - pos));
	memcpy(buf, base + pos, count);

	return count;
}