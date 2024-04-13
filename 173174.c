static void bnx2x_init_eq_ring(struct bnx2x *bp)
{
	int i;
	for (i = 1; i <= NUM_EQ_PAGES; i++) {
		union event_ring_elem *elem =
			&bp->eq_ring[EQ_DESC_CNT_PAGE * i - 1];

		elem->next_page.addr.hi =
			cpu_to_le32(U64_HI(bp->eq_mapping +
				   BCM_PAGE_SIZE * (i % NUM_EQ_PAGES)));
		elem->next_page.addr.lo =
			cpu_to_le32(U64_LO(bp->eq_mapping +
				   BCM_PAGE_SIZE*(i % NUM_EQ_PAGES)));
	}
	bp->eq_cons = 0;
	bp->eq_prod = NUM_EQ_DESC;
	bp->eq_cons_sb = BNX2X_EQ_INDEX;
	/* we want a warning message before it gets wrought... */
	atomic_set(&bp->eq_spq_left,
		min_t(int, MAX_SP_DESC_CNT - MAX_SPQ_PENDING, NUM_EQ_DESC) - 1);
}