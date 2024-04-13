static void update_pkru_bitmask(struct kvm_mmu *mmu)
{
	unsigned bit;
	bool wp;

	mmu->pkru_mask = 0;

	if (!is_cr4_pke(mmu))
		return;

	wp = is_cr0_wp(mmu);

	for (bit = 0; bit < ARRAY_SIZE(mmu->permissions); ++bit) {
		unsigned pfec, pkey_bits;
		bool check_pkey, check_write, ff, uf, wf, pte_user;

		pfec = bit << 1;
		ff = pfec & PFERR_FETCH_MASK;
		uf = pfec & PFERR_USER_MASK;
		wf = pfec & PFERR_WRITE_MASK;

		/* PFEC.RSVD is replaced by ACC_USER_MASK. */
		pte_user = pfec & PFERR_RSVD_MASK;

		/*
		 * Only need to check the access which is not an
		 * instruction fetch and is to a user page.
		 */
		check_pkey = (!ff && pte_user);
		/*
		 * write access is controlled by PKRU if it is a
		 * user access or CR0.WP = 1.
		 */
		check_write = check_pkey && wf && (uf || wp);

		/* PKRU.AD stops both read and write access. */
		pkey_bits = !!check_pkey;
		/* PKRU.WD stops write access. */
		pkey_bits |= (!!check_write) << 1;

		mmu->pkru_mask |= (pkey_bits & 3) << pfec;
	}
}