static inline void kvm_mod_used_mmu_pages(struct kvm *kvm, long nr)
{
	kvm->arch.n_used_mmu_pages += nr;
	percpu_counter_add(&kvm_total_used_mmu_pages, nr);
}