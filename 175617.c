static void __intel_vgpu_release(struct intel_vgpu *vgpu)
{
	struct kvmgt_guest_info *info;
	int ret;

	if (!handle_valid(vgpu->handle))
		return;

	if (atomic_cmpxchg(&vgpu->vdev.released, 0, 1))
		return;

	intel_gvt_ops->vgpu_release(vgpu);

	ret = vfio_unregister_notifier(mdev_dev(vgpu->vdev.mdev), VFIO_IOMMU_NOTIFY,
					&vgpu->vdev.iommu_notifier);
	WARN(ret, "vfio_unregister_notifier for iommu failed: %d\n", ret);

	ret = vfio_unregister_notifier(mdev_dev(vgpu->vdev.mdev), VFIO_GROUP_NOTIFY,
					&vgpu->vdev.group_notifier);
	WARN(ret, "vfio_unregister_notifier for group failed: %d\n", ret);

	info = (struct kvmgt_guest_info *)vgpu->handle;
	kvmgt_guest_exit(info);

	intel_vgpu_release_msi_eventfd_ctx(vgpu);

	vgpu->vdev.kvm = NULL;
	vgpu->handle = 0;
}