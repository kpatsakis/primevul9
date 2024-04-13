static bool kvm_zap_rmapp(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			  const struct kvm_memory_slot *slot)
{
	return pte_list_destroy(kvm, rmap_head);
}