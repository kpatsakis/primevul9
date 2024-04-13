static void mark_mmio_spte(struct kvm_vcpu *vcpu, u64 *sptep, u64 gfn,
			   unsigned int access)
{
	u64 spte = make_mmio_spte(vcpu, gfn, access);

	trace_mark_mmio_spte(sptep, gfn, spte);
	mmu_spte_set(sptep, spte);
}