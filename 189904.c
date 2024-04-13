__reset_rsvds_bits_mask(struct rsvd_bits_validate *rsvd_check,
			u64 pa_bits_rsvd, int level, bool nx, bool gbpages,
			bool pse, bool amd)
{
	u64 gbpages_bit_rsvd = 0;
	u64 nonleaf_bit8_rsvd = 0;
	u64 high_bits_rsvd;

	rsvd_check->bad_mt_xwr = 0;

	if (!gbpages)
		gbpages_bit_rsvd = rsvd_bits(7, 7);

	if (level == PT32E_ROOT_LEVEL)
		high_bits_rsvd = pa_bits_rsvd & rsvd_bits(0, 62);
	else
		high_bits_rsvd = pa_bits_rsvd & rsvd_bits(0, 51);

	/* Note, NX doesn't exist in PDPTEs, this is handled below. */
	if (!nx)
		high_bits_rsvd |= rsvd_bits(63, 63);

	/*
	 * Non-leaf PML4Es and PDPEs reserve bit 8 (which would be the G bit for
	 * leaf entries) on AMD CPUs only.
	 */
	if (amd)
		nonleaf_bit8_rsvd = rsvd_bits(8, 8);

	switch (level) {
	case PT32_ROOT_LEVEL:
		/* no rsvd bits for 2 level 4K page table entries */
		rsvd_check->rsvd_bits_mask[0][1] = 0;
		rsvd_check->rsvd_bits_mask[0][0] = 0;
		rsvd_check->rsvd_bits_mask[1][0] =
			rsvd_check->rsvd_bits_mask[0][0];

		if (!pse) {
			rsvd_check->rsvd_bits_mask[1][1] = 0;
			break;
		}

		if (is_cpuid_PSE36())
			/* 36bits PSE 4MB page */
			rsvd_check->rsvd_bits_mask[1][1] = rsvd_bits(17, 21);
		else
			/* 32 bits PSE 4MB page */
			rsvd_check->rsvd_bits_mask[1][1] = rsvd_bits(13, 21);
		break;
	case PT32E_ROOT_LEVEL:
		rsvd_check->rsvd_bits_mask[0][2] = rsvd_bits(63, 63) |
						   high_bits_rsvd |
						   rsvd_bits(5, 8) |
						   rsvd_bits(1, 2);	/* PDPTE */
		rsvd_check->rsvd_bits_mask[0][1] = high_bits_rsvd;	/* PDE */
		rsvd_check->rsvd_bits_mask[0][0] = high_bits_rsvd;	/* PTE */
		rsvd_check->rsvd_bits_mask[1][1] = high_bits_rsvd |
						   rsvd_bits(13, 20);	/* large page */
		rsvd_check->rsvd_bits_mask[1][0] =
			rsvd_check->rsvd_bits_mask[0][0];
		break;
	case PT64_ROOT_5LEVEL:
		rsvd_check->rsvd_bits_mask[0][4] = high_bits_rsvd |
						   nonleaf_bit8_rsvd |
						   rsvd_bits(7, 7);
		rsvd_check->rsvd_bits_mask[1][4] =
			rsvd_check->rsvd_bits_mask[0][4];
		fallthrough;
	case PT64_ROOT_4LEVEL:
		rsvd_check->rsvd_bits_mask[0][3] = high_bits_rsvd |
						   nonleaf_bit8_rsvd |
						   rsvd_bits(7, 7);
		rsvd_check->rsvd_bits_mask[0][2] = high_bits_rsvd |
						   gbpages_bit_rsvd;
		rsvd_check->rsvd_bits_mask[0][1] = high_bits_rsvd;
		rsvd_check->rsvd_bits_mask[0][0] = high_bits_rsvd;
		rsvd_check->rsvd_bits_mask[1][3] =
			rsvd_check->rsvd_bits_mask[0][3];
		rsvd_check->rsvd_bits_mask[1][2] = high_bits_rsvd |
						   gbpages_bit_rsvd |
						   rsvd_bits(13, 29);
		rsvd_check->rsvd_bits_mask[1][1] = high_bits_rsvd |
						   rsvd_bits(13, 20); /* large page */
		rsvd_check->rsvd_bits_mask[1][0] =
			rsvd_check->rsvd_bits_mask[0][0];
		break;
	}
}