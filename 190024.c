void kvm_flush_remote_tlbs_with_address(struct kvm *kvm,
		u64 start_gfn, u64 pages)
{
	struct kvm_tlb_range range;

	range.start_gfn = start_gfn;
	range.pages = pages;

	kvm_flush_remote_tlbs_with_range(kvm, &range);
}