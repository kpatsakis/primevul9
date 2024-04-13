static int make_mmu_pages_available(struct kvm_vcpu *vcpu)
{
	unsigned long avail = kvm_mmu_available_pages(vcpu->kvm);

	if (likely(avail >= KVM_MIN_FREE_MMU_PAGES))
		return 0;

	kvm_mmu_zap_oldest_mmu_pages(vcpu->kvm, KVM_REFILL_PAGES - avail);

	/*
	 * Note, this check is intentionally soft, it only guarantees that one
	 * page is available, while the caller may end up allocating as many as
	 * four pages, e.g. for PAE roots or for 5-level paging.  Temporarily
	 * exceeding the (arbitrary by default) limit will not harm the host,
	 * being too aggressive may unnecessarily kill the guest, and getting an
	 * exact count is far more trouble than it's worth, especially in the
	 * page fault paths.
	 */
	if (!kvm_mmu_available_pages(vcpu->kvm))
		return -ENOSPC;
	return 0;
}