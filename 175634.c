static long intel_vgpu_ioctl(struct mdev_device *mdev, unsigned int cmd,
			     unsigned long arg)
{
	struct intel_vgpu *vgpu = mdev_get_drvdata(mdev);
	unsigned long minsz;

	gvt_dbg_core("vgpu%d ioctl, cmd: %d\n", vgpu->id, cmd);

	if (cmd == VFIO_DEVICE_GET_INFO) {
		struct vfio_device_info info;

		minsz = offsetofend(struct vfio_device_info, num_irqs);

		if (copy_from_user(&info, (void __user *)arg, minsz))
			return -EFAULT;

		if (info.argsz < minsz)
			return -EINVAL;

		info.flags = VFIO_DEVICE_FLAGS_PCI;
		info.flags |= VFIO_DEVICE_FLAGS_RESET;
		info.num_regions = VFIO_PCI_NUM_REGIONS +
				vgpu->vdev.num_regions;
		info.num_irqs = VFIO_PCI_NUM_IRQS;

		return copy_to_user((void __user *)arg, &info, minsz) ?
			-EFAULT : 0;

	} else if (cmd == VFIO_DEVICE_GET_REGION_INFO) {
		struct vfio_region_info info;
		struct vfio_info_cap caps = { .buf = NULL, .size = 0 };
		unsigned int i;
		int ret;
		struct vfio_region_info_cap_sparse_mmap *sparse = NULL;
		size_t size;
		int nr_areas = 1;
		int cap_type_id;

		minsz = offsetofend(struct vfio_region_info, offset);

		if (copy_from_user(&info, (void __user *)arg, minsz))
			return -EFAULT;

		if (info.argsz < minsz)
			return -EINVAL;

		switch (info.index) {
		case VFIO_PCI_CONFIG_REGION_INDEX:
			info.offset = VFIO_PCI_INDEX_TO_OFFSET(info.index);
			info.size = vgpu->gvt->device_info.cfg_space_size;
			info.flags = VFIO_REGION_INFO_FLAG_READ |
				     VFIO_REGION_INFO_FLAG_WRITE;
			break;
		case VFIO_PCI_BAR0_REGION_INDEX:
			info.offset = VFIO_PCI_INDEX_TO_OFFSET(info.index);
			info.size = vgpu->cfg_space.bar[info.index].size;
			if (!info.size) {
				info.flags = 0;
				break;
			}

			info.flags = VFIO_REGION_INFO_FLAG_READ |
				     VFIO_REGION_INFO_FLAG_WRITE;
			break;
		case VFIO_PCI_BAR1_REGION_INDEX:
			info.offset = VFIO_PCI_INDEX_TO_OFFSET(info.index);
			info.size = 0;
			info.flags = 0;
			break;
		case VFIO_PCI_BAR2_REGION_INDEX:
			info.offset = VFIO_PCI_INDEX_TO_OFFSET(info.index);
			info.flags = VFIO_REGION_INFO_FLAG_CAPS |
					VFIO_REGION_INFO_FLAG_MMAP |
					VFIO_REGION_INFO_FLAG_READ |
					VFIO_REGION_INFO_FLAG_WRITE;
			info.size = gvt_aperture_sz(vgpu->gvt);

			size = sizeof(*sparse) +
					(nr_areas * sizeof(*sparse->areas));
			sparse = kzalloc(size, GFP_KERNEL);
			if (!sparse)
				return -ENOMEM;

			sparse->header.id = VFIO_REGION_INFO_CAP_SPARSE_MMAP;
			sparse->header.version = 1;
			sparse->nr_areas = nr_areas;
			cap_type_id = VFIO_REGION_INFO_CAP_SPARSE_MMAP;
			sparse->areas[0].offset =
					PAGE_ALIGN(vgpu_aperture_offset(vgpu));
			sparse->areas[0].size = vgpu_aperture_sz(vgpu);
			break;

		case VFIO_PCI_BAR3_REGION_INDEX ... VFIO_PCI_BAR5_REGION_INDEX:
			info.offset = VFIO_PCI_INDEX_TO_OFFSET(info.index);
			info.size = 0;
			info.flags = 0;

			gvt_dbg_core("get region info bar:%d\n", info.index);
			break;

		case VFIO_PCI_ROM_REGION_INDEX:
		case VFIO_PCI_VGA_REGION_INDEX:
			info.offset = VFIO_PCI_INDEX_TO_OFFSET(info.index);
			info.size = 0;
			info.flags = 0;

			gvt_dbg_core("get region info index:%d\n", info.index);
			break;
		default:
			{
				struct vfio_region_info_cap_type cap_type = {
					.header.id = VFIO_REGION_INFO_CAP_TYPE,
					.header.version = 1 };

				if (info.index >= VFIO_PCI_NUM_REGIONS +
						vgpu->vdev.num_regions)
					return -EINVAL;
				info.index =
					array_index_nospec(info.index,
							VFIO_PCI_NUM_REGIONS +
							vgpu->vdev.num_regions);

				i = info.index - VFIO_PCI_NUM_REGIONS;

				info.offset =
					VFIO_PCI_INDEX_TO_OFFSET(info.index);
				info.size = vgpu->vdev.region[i].size;
				info.flags = vgpu->vdev.region[i].flags;

				cap_type.type = vgpu->vdev.region[i].type;
				cap_type.subtype = vgpu->vdev.region[i].subtype;

				ret = vfio_info_add_capability(&caps,
							&cap_type.header,
							sizeof(cap_type));
				if (ret)
					return ret;
			}
		}

		if ((info.flags & VFIO_REGION_INFO_FLAG_CAPS) && sparse) {
			switch (cap_type_id) {
			case VFIO_REGION_INFO_CAP_SPARSE_MMAP:
				ret = vfio_info_add_capability(&caps,
					&sparse->header, sizeof(*sparse) +
					(sparse->nr_areas *
						sizeof(*sparse->areas)));
				if (ret) {
					kfree(sparse);
					return ret;
				}
				break;
			default:
				kfree(sparse);
				return -EINVAL;
			}
		}

		if (caps.size) {
			info.flags |= VFIO_REGION_INFO_FLAG_CAPS;
			if (info.argsz < sizeof(info) + caps.size) {
				info.argsz = sizeof(info) + caps.size;
				info.cap_offset = 0;
			} else {
				vfio_info_cap_shift(&caps, sizeof(info));
				if (copy_to_user((void __user *)arg +
						  sizeof(info), caps.buf,
						  caps.size)) {
					kfree(caps.buf);
					kfree(sparse);
					return -EFAULT;
				}
				info.cap_offset = sizeof(info);
			}

			kfree(caps.buf);
		}

		kfree(sparse);
		return copy_to_user((void __user *)arg, &info, minsz) ?
			-EFAULT : 0;
	} else if (cmd == VFIO_DEVICE_GET_IRQ_INFO) {
		struct vfio_irq_info info;

		minsz = offsetofend(struct vfio_irq_info, count);

		if (copy_from_user(&info, (void __user *)arg, minsz))
			return -EFAULT;

		if (info.argsz < minsz || info.index >= VFIO_PCI_NUM_IRQS)
			return -EINVAL;

		switch (info.index) {
		case VFIO_PCI_INTX_IRQ_INDEX:
		case VFIO_PCI_MSI_IRQ_INDEX:
			break;
		default:
			return -EINVAL;
		}

		info.flags = VFIO_IRQ_INFO_EVENTFD;

		info.count = intel_vgpu_get_irq_count(vgpu, info.index);

		if (info.index == VFIO_PCI_INTX_IRQ_INDEX)
			info.flags |= (VFIO_IRQ_INFO_MASKABLE |
				       VFIO_IRQ_INFO_AUTOMASKED);
		else
			info.flags |= VFIO_IRQ_INFO_NORESIZE;

		return copy_to_user((void __user *)arg, &info, minsz) ?
			-EFAULT : 0;
	} else if (cmd == VFIO_DEVICE_SET_IRQS) {
		struct vfio_irq_set hdr;
		u8 *data = NULL;
		int ret = 0;
		size_t data_size = 0;

		minsz = offsetofend(struct vfio_irq_set, count);

		if (copy_from_user(&hdr, (void __user *)arg, minsz))
			return -EFAULT;

		if (!(hdr.flags & VFIO_IRQ_SET_DATA_NONE)) {
			int max = intel_vgpu_get_irq_count(vgpu, hdr.index);

			ret = vfio_set_irqs_validate_and_prepare(&hdr, max,
						VFIO_PCI_NUM_IRQS, &data_size);
			if (ret) {
				gvt_vgpu_err("intel:vfio_set_irqs_validate_and_prepare failed\n");
				return -EINVAL;
			}
			if (data_size) {
				data = memdup_user((void __user *)(arg + minsz),
						   data_size);
				if (IS_ERR(data))
					return PTR_ERR(data);
			}
		}

		ret = intel_vgpu_set_irqs(vgpu, hdr.flags, hdr.index,
					hdr.start, hdr.count, data);
		kfree(data);

		return ret;
	} else if (cmd == VFIO_DEVICE_RESET) {
		intel_gvt_ops->vgpu_reset(vgpu);
		return 0;
	} else if (cmd == VFIO_DEVICE_QUERY_GFX_PLANE) {
		struct vfio_device_gfx_plane_info dmabuf;
		int ret = 0;

		minsz = offsetofend(struct vfio_device_gfx_plane_info,
				    dmabuf_id);
		if (copy_from_user(&dmabuf, (void __user *)arg, minsz))
			return -EFAULT;
		if (dmabuf.argsz < minsz)
			return -EINVAL;

		ret = intel_gvt_ops->vgpu_query_plane(vgpu, &dmabuf);
		if (ret != 0)
			return ret;

		return copy_to_user((void __user *)arg, &dmabuf, minsz) ?
								-EFAULT : 0;
	} else if (cmd == VFIO_DEVICE_GET_GFX_DMABUF) {
		__u32 dmabuf_id;
		__s32 dmabuf_fd;

		if (get_user(dmabuf_id, (__u32 __user *)arg))
			return -EFAULT;

		dmabuf_fd = intel_gvt_ops->vgpu_get_dmabuf(vgpu, dmabuf_id);
		return dmabuf_fd;

	}

	return -ENOTTY;
}