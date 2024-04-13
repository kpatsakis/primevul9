static void rmap_add(struct kvm_vcpu *vcpu, struct kvm_memory_slot *slot,
		     u64 *spte, gfn_t gfn)
{
	struct kvm_mmu_page *sp;
	struct kvm_rmap_head *rmap_head;
	int rmap_count;

	sp = sptep_to_sp(spte);
	kvm_mmu_page_set_gfn(sp, spte - sp->spt, gfn);
	rmap_head = gfn_to_rmap(gfn, sp->role.level, slot);
	rmap_count = pte_list_add(vcpu, spte, rmap_head);

	if (rmap_count > RMAP_RECYCLE_THRESHOLD) {
		kvm_unmap_rmapp(vcpu->kvm, rmap_head, NULL, gfn, sp->role.level, __pte(0));
		kvm_flush_remote_tlbs_with_address(
				vcpu->kvm, sp->gfn, KVM_PAGES_PER_HPAGE(sp->role.level));
	}
}