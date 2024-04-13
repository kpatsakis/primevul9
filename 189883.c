static bool sync_mmio_spte(struct kvm_vcpu *vcpu, u64 *sptep, gfn_t gfn,
			   unsigned int access)
{
	if (unlikely(is_mmio_spte(*sptep))) {
		if (gfn != get_mmio_spte_gfn(*sptep)) {
			mmu_spte_clear_no_track(sptep);
			return true;
		}

		mark_mmio_spte(vcpu, sptep, gfn, access);
		return true;
	}

	return false;
}