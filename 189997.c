void kvm_mmu_change_mmu_pages(struct kvm *kvm, unsigned long goal_nr_mmu_pages)
{
	write_lock(&kvm->mmu_lock);

	if (kvm->arch.n_used_mmu_pages > goal_nr_mmu_pages) {
		kvm_mmu_zap_oldest_mmu_pages(kvm, kvm->arch.n_used_mmu_pages -
						  goal_nr_mmu_pages);

		goal_nr_mmu_pages = kvm->arch.n_used_mmu_pages;
	}

	kvm->arch.n_max_mmu_pages = goal_nr_mmu_pages;

	write_unlock(&kvm->mmu_lock);
}