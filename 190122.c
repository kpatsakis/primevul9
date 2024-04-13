static inline bool kvm_available_flush_tlb_with_range(void)
{
	return kvm_x86_ops.tlb_remote_flush_with_range;
}