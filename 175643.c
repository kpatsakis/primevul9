static bool gtt_entry(struct mdev_device *mdev, loff_t *ppos)
{
	struct intel_vgpu *vgpu = mdev_get_drvdata(mdev);
	unsigned int index = VFIO_PCI_OFFSET_TO_INDEX(*ppos);
	struct intel_gvt *gvt = vgpu->gvt;
	int offset;

	/* Only allow MMIO GGTT entry access */
	if (index != PCI_BASE_ADDRESS_0)
		return false;

	offset = (u64)(*ppos & VFIO_PCI_OFFSET_MASK) -
		intel_vgpu_get_bar_gpa(vgpu, PCI_BASE_ADDRESS_0);

	return (offset >= gvt->device_info.gtt_start_offset &&
		offset < gvt->device_info.gtt_start_offset + gvt_ggtt_sz(gvt)) ?
			true : false;
}