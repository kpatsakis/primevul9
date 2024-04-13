static ssize_t intel_vgpu_rw(struct mdev_device *mdev, char *buf,
			size_t count, loff_t *ppos, bool is_write)
{
	struct intel_vgpu *vgpu = mdev_get_drvdata(mdev);
	unsigned int index = VFIO_PCI_OFFSET_TO_INDEX(*ppos);
	uint64_t pos = *ppos & VFIO_PCI_OFFSET_MASK;
	int ret = -EINVAL;


	if (index >= VFIO_PCI_NUM_REGIONS + vgpu->vdev.num_regions) {
		gvt_vgpu_err("invalid index: %u\n", index);
		return -EINVAL;
	}

	switch (index) {
	case VFIO_PCI_CONFIG_REGION_INDEX:
		if (is_write)
			ret = intel_gvt_ops->emulate_cfg_write(vgpu, pos,
						buf, count);
		else
			ret = intel_gvt_ops->emulate_cfg_read(vgpu, pos,
						buf, count);
		break;
	case VFIO_PCI_BAR0_REGION_INDEX:
		ret = intel_vgpu_bar_rw(vgpu, PCI_BASE_ADDRESS_0, pos,
					buf, count, is_write);
		break;
	case VFIO_PCI_BAR2_REGION_INDEX:
		ret = intel_vgpu_aperture_rw(vgpu, pos, buf, count, is_write);
		break;
	case VFIO_PCI_BAR1_REGION_INDEX:
	case VFIO_PCI_BAR3_REGION_INDEX:
	case VFIO_PCI_BAR4_REGION_INDEX:
	case VFIO_PCI_BAR5_REGION_INDEX:
	case VFIO_PCI_VGA_REGION_INDEX:
	case VFIO_PCI_ROM_REGION_INDEX:
		break;
	default:
		if (index >= VFIO_PCI_NUM_REGIONS + vgpu->vdev.num_regions)
			return -EINVAL;

		index -= VFIO_PCI_NUM_REGIONS;
		return vgpu->vdev.region[index].ops->rw(vgpu, buf, count,
				ppos, is_write);
	}

	return ret == 0 ? count : ret;
}