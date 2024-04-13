static void kvm_mmu_zap_all_fast(struct kvm *kvm)
{
	lockdep_assert_held(&kvm->slots_lock);

	write_lock(&kvm->mmu_lock);
	trace_kvm_mmu_zap_all_fast(kvm);

	/*
	 * Toggle mmu_valid_gen between '0' and '1'.  Because slots_lock is
	 * held for the entire duration of zapping obsolete pages, it's
	 * impossible for there to be multiple invalid generations associated
	 * with *valid* shadow pages at any given time, i.e. there is exactly
	 * one valid generation and (at most) one invalid generation.
	 */
	kvm->arch.mmu_valid_gen = kvm->arch.mmu_valid_gen ? 0 : 1;

	/*
	 * In order to ensure all vCPUs drop their soon-to-be invalid roots,
	 * invalidating TDP MMU roots must be done while holding mmu_lock for
	 * write and in the same critical section as making the reload request,
	 * e.g. before kvm_zap_obsolete_pages() could drop mmu_lock and yield.
	 */
	if (is_tdp_mmu_enabled(kvm))
		kvm_tdp_mmu_invalidate_all_roots(kvm);

	/*
	 * Notify all vcpus to reload its shadow page table and flush TLB.
	 * Then all vcpus will switch to new shadow page table with the new
	 * mmu_valid_gen.
	 *
	 * Note: we need to do this under the protection of mmu_lock,
	 * otherwise, vcpu would purge shadow page but miss tlb flush.
	 */
	kvm_make_all_cpus_request(kvm, KVM_REQ_MMU_FREE_OBSOLETE_ROOTS);

	kvm_zap_obsolete_pages(kvm);

	write_unlock(&kvm->mmu_lock);

	/*
	 * Zap the invalidated TDP MMU roots, all SPTEs must be dropped before
	 * returning to the caller, e.g. if the zap is in response to a memslot
	 * deletion, mmu_notifier callbacks will be unable to reach the SPTEs
	 * associated with the deleted memslot once the update completes, and
	 * Deferring the zap until the final reference to the root is put would
	 * lead to use-after-free.
	 */
	if (is_tdp_mmu_enabled(kvm))
		kvm_tdp_mmu_zap_invalidated_roots(kvm);
}