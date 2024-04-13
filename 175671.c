static int intel_vgpu_open(struct mdev_device *mdev)
{
	struct intel_vgpu *vgpu = mdev_get_drvdata(mdev);
	unsigned long events;
	int ret;

	vgpu->vdev.iommu_notifier.notifier_call = intel_vgpu_iommu_notifier;
	vgpu->vdev.group_notifier.notifier_call = intel_vgpu_group_notifier;

	events = VFIO_IOMMU_NOTIFY_DMA_UNMAP;
	ret = vfio_register_notifier(mdev_dev(mdev), VFIO_IOMMU_NOTIFY, &events,
				&vgpu->vdev.iommu_notifier);
	if (ret != 0) {
		gvt_vgpu_err("vfio_register_notifier for iommu failed: %d\n",
			ret);
		goto out;
	}

	events = VFIO_GROUP_NOTIFY_SET_KVM;
	ret = vfio_register_notifier(mdev_dev(mdev), VFIO_GROUP_NOTIFY, &events,
				&vgpu->vdev.group_notifier);
	if (ret != 0) {
		gvt_vgpu_err("vfio_register_notifier for group failed: %d\n",
			ret);
		goto undo_iommu;
	}

	ret = kvmgt_guest_init(mdev);
	if (ret)
		goto undo_group;

	intel_gvt_ops->vgpu_activate(vgpu);

	atomic_set(&vgpu->vdev.released, 0);
	return ret;

undo_group:
	vfio_unregister_notifier(mdev_dev(mdev), VFIO_GROUP_NOTIFY,
					&vgpu->vdev.group_notifier);

undo_iommu:
	vfio_unregister_notifier(mdev_dev(mdev), VFIO_IOMMU_NOTIFY,
					&vgpu->vdev.iommu_notifier);
out:
	return ret;
}