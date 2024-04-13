static bool kvm_mmu_remote_flush_or_zap(struct kvm *kvm,
					struct list_head *invalid_list,
					bool remote_flush)
{
	if (!remote_flush && list_empty(invalid_list))
		return false;

	if (!list_empty(invalid_list))
		kvm_mmu_commit_zap_page(kvm, invalid_list);
	else
		kvm_flush_remote_tlbs(kvm);
	return true;
}