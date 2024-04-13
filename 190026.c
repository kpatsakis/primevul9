static bool check_mmio_spte(struct kvm_vcpu *vcpu, u64 spte)
{
	u64 kvm_gen, spte_gen, gen;

	gen = kvm_vcpu_memslots(vcpu)->generation;
	if (unlikely(gen & KVM_MEMSLOT_GEN_UPDATE_IN_PROGRESS))
		return false;

	kvm_gen = gen & MMIO_SPTE_GEN_MASK;
	spte_gen = get_mmio_spte_generation(spte);

	trace_check_mmio_spte(spte, kvm_gen, spte_gen);
	return likely(kvm_gen == spte_gen);
}