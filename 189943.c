static void kvm_flush_remote_tlbs_with_range(struct kvm *kvm,
		struct kvm_tlb_range *range)
{
	int ret = -ENOTSUPP;

	if (range && kvm_x86_ops.tlb_remote_flush_with_range)
		ret = static_call(kvm_x86_tlb_remote_flush_with_range)(kvm, range);

	if (ret)
		kvm_flush_remote_tlbs(kvm);
}