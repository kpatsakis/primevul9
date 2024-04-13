__reset_rsvds_bits_mask_ept(struct rsvd_bits_validate *rsvd_check,
			    u64 pa_bits_rsvd, bool execonly, int huge_page_level)
{
	u64 high_bits_rsvd = pa_bits_rsvd & rsvd_bits(0, 51);
	u64 large_1g_rsvd = 0, large_2m_rsvd = 0;
	u64 bad_mt_xwr;

	if (huge_page_level < PG_LEVEL_1G)
		large_1g_rsvd = rsvd_bits(7, 7);
	if (huge_page_level < PG_LEVEL_2M)
		large_2m_rsvd = rsvd_bits(7, 7);

	rsvd_check->rsvd_bits_mask[0][4] = high_bits_rsvd | rsvd_bits(3, 7);
	rsvd_check->rsvd_bits_mask[0][3] = high_bits_rsvd | rsvd_bits(3, 7);
	rsvd_check->rsvd_bits_mask[0][2] = high_bits_rsvd | rsvd_bits(3, 6) | large_1g_rsvd;
	rsvd_check->rsvd_bits_mask[0][1] = high_bits_rsvd | rsvd_bits(3, 6) | large_2m_rsvd;
	rsvd_check->rsvd_bits_mask[0][0] = high_bits_rsvd;

	/* large page */
	rsvd_check->rsvd_bits_mask[1][4] = rsvd_check->rsvd_bits_mask[0][4];
	rsvd_check->rsvd_bits_mask[1][3] = rsvd_check->rsvd_bits_mask[0][3];
	rsvd_check->rsvd_bits_mask[1][2] = high_bits_rsvd | rsvd_bits(12, 29) | large_1g_rsvd;
	rsvd_check->rsvd_bits_mask[1][1] = high_bits_rsvd | rsvd_bits(12, 20) | large_2m_rsvd;
	rsvd_check->rsvd_bits_mask[1][0] = rsvd_check->rsvd_bits_mask[0][0];

	bad_mt_xwr = 0xFFull << (2 * 8);	/* bits 3..5 must not be 2 */
	bad_mt_xwr |= 0xFFull << (3 * 8);	/* bits 3..5 must not be 3 */
	bad_mt_xwr |= 0xFFull << (7 * 8);	/* bits 3..5 must not be 7 */
	bad_mt_xwr |= REPEAT_BYTE(1ull << 2);	/* bits 0..2 must not be 010 */
	bad_mt_xwr |= REPEAT_BYTE(1ull << 6);	/* bits 0..2 must not be 110 */
	if (!execonly) {
		/* bits 0..2 must not be 100 unless VMX capabilities allow it */
		bad_mt_xwr |= REPEAT_BYTE(1ull << 4);
	}
	rsvd_check->bad_mt_xwr = bad_mt_xwr;
}