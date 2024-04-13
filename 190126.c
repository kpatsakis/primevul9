static bool __drop_large_spte(struct kvm *kvm, u64 *sptep)
{
	if (is_large_pte(*sptep)) {
		WARN_ON(sptep_to_sp(sptep)->role.level == PG_LEVEL_4K);
		drop_spte(kvm, sptep);
		return true;
	}

	return false;
}