static int kvmgt_guest_init(struct mdev_device *mdev)
{
	struct kvmgt_guest_info *info;
	struct intel_vgpu *vgpu;
	struct kvm *kvm;

	vgpu = mdev_get_drvdata(mdev);
	if (handle_valid(vgpu->handle))
		return -EEXIST;

	kvm = vgpu->vdev.kvm;
	if (!kvm || kvm->mm != current->mm) {
		gvt_vgpu_err("KVM is required to use Intel vGPU\n");
		return -ESRCH;
	}

	if (__kvmgt_vgpu_exist(vgpu, kvm))
		return -EEXIST;

	info = vzalloc(sizeof(struct kvmgt_guest_info));
	if (!info)
		return -ENOMEM;

	vgpu->handle = (unsigned long)info;
	info->vgpu = vgpu;
	info->kvm = kvm;
	kvm_get_kvm(info->kvm);

	kvmgt_protect_table_init(info);
	gvt_cache_init(vgpu);

	init_completion(&vgpu->vblank_done);

	info->track_node.track_write = kvmgt_page_track_write;
	info->track_node.track_flush_slot = kvmgt_page_track_flush_slot;
	kvm_page_track_register_notifier(kvm, &info->track_node);

	info->debugfs_cache_entries = debugfs_create_ulong(
						"kvmgt_nr_cache_entries",
						0444, vgpu->debugfs,
						&vgpu->vdev.nr_cache_entries);
	if (!info->debugfs_cache_entries)
		gvt_vgpu_err("Cannot create kvmgt debugfs entry\n");

	return 0;
}