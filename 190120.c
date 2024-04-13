mmu_shrink_scan(struct shrinker *shrink, struct shrink_control *sc)
{
	struct kvm *kvm;
	int nr_to_scan = sc->nr_to_scan;
	unsigned long freed = 0;

	mutex_lock(&kvm_lock);

	list_for_each_entry(kvm, &vm_list, vm_list) {
		int idx;
		LIST_HEAD(invalid_list);

		/*
		 * Never scan more than sc->nr_to_scan VM instances.
		 * Will not hit this condition practically since we do not try
		 * to shrink more than one VM and it is very unlikely to see
		 * !n_used_mmu_pages so many times.
		 */
		if (!nr_to_scan--)
			break;
		/*
		 * n_used_mmu_pages is accessed without holding kvm->mmu_lock
		 * here. We may skip a VM instance errorneosly, but we do not
		 * want to shrink a VM that only started to populate its MMU
		 * anyway.
		 */
		if (!kvm->arch.n_used_mmu_pages &&
		    !kvm_has_zapped_obsolete_pages(kvm))
			continue;

		idx = srcu_read_lock(&kvm->srcu);
		write_lock(&kvm->mmu_lock);

		if (kvm_has_zapped_obsolete_pages(kvm)) {
			kvm_mmu_commit_zap_page(kvm,
			      &kvm->arch.zapped_obsolete_pages);
			goto unlock;
		}

		freed = kvm_mmu_zap_oldest_mmu_pages(kvm, sc->nr_to_scan);

unlock:
		write_unlock(&kvm->mmu_lock);
		srcu_read_unlock(&kvm->srcu, idx);

		/*
		 * unfair on small ones
		 * per-vm shrinkers cry out
		 * sadness comes quickly
		 */
		list_move_tail(&kvm->vm_list, &vm_list);
		break;
	}

	mutex_unlock(&kvm_lock);
	return freed;
}