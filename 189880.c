static bool kvm_unmap_rmapp(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			    struct kvm_memory_slot *slot, gfn_t gfn, int level,
			    pte_t unused)
{
	return kvm_zap_rmapp(kvm, rmap_head, slot);
}