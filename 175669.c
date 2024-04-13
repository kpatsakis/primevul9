static bool kvmgt_guest_exit(struct kvmgt_guest_info *info)
{
	debugfs_remove(info->debugfs_cache_entries);

	kvm_page_track_unregister_notifier(info->kvm, &info->track_node);
	kvm_put_kvm(info->kvm);
	kvmgt_protect_table_destroy(info);
	gvt_cache_destroy(info->vgpu);
	vfree(info);

	return true;
}