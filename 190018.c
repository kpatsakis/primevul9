void kvm_zap_gfn_range(struct kvm *kvm, gfn_t gfn_start, gfn_t gfn_end)
{
	bool flush;
	int i;

	if (WARN_ON_ONCE(gfn_end <= gfn_start))
		return;

	write_lock(&kvm->mmu_lock);

	kvm_inc_notifier_count(kvm, gfn_start, gfn_end);

	flush = __kvm_zap_rmaps(kvm, gfn_start, gfn_end);

	if (is_tdp_mmu_enabled(kvm)) {
		for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++)
			flush = kvm_tdp_mmu_zap_leafs(kvm, i, gfn_start,
						      gfn_end, true, flush);
	}

	if (flush)
		kvm_flush_remote_tlbs_with_address(kvm, gfn_start,
						   gfn_end - gfn_start);

	kvm_dec_notifier_count(kvm, gfn_start, gfn_end);

	write_unlock(&kvm->mmu_lock);
}