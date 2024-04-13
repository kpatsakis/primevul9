static bool slot_rmap_write_protect(struct kvm *kvm,
				    struct kvm_rmap_head *rmap_head,
				    const struct kvm_memory_slot *slot)
{
	return rmap_write_protect(rmap_head, false);
}