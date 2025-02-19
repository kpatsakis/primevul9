static void dump_sdma_state(struct sdma_engine *sde)
{
	struct hw_sdma_desc *descqp;
	u64 desc[2];
	u64 addr;
	u8 gen;
	u16 len;
	u16 head, tail, cnt;

	head = sde->descq_head & sde->sdma_mask;
	tail = sde->descq_tail & sde->sdma_mask;
	cnt = sdma_descq_freecnt(sde);

	dd_dev_err(sde->dd,
		   "SDMA (%u) descq_head: %u descq_tail: %u freecnt: %u FLE %d\n",
		   sde->this_idx, head, tail, cnt,
		   !list_empty(&sde->flushlist));

	/* print info for each entry in the descriptor queue */
	while (head != tail) {
		char flags[6] = { 'x', 'x', 'x', 'x', 0 };

		descqp = &sde->descq[head];
		desc[0] = le64_to_cpu(descqp->qw[0]);
		desc[1] = le64_to_cpu(descqp->qw[1]);
		flags[0] = (desc[1] & SDMA_DESC1_INT_REQ_FLAG) ? 'I' : '-';
		flags[1] = (desc[1] & SDMA_DESC1_HEAD_TO_HOST_FLAG) ?
				'H' : '-';
		flags[2] = (desc[0] & SDMA_DESC0_FIRST_DESC_FLAG) ? 'F' : '-';
		flags[3] = (desc[0] & SDMA_DESC0_LAST_DESC_FLAG) ? 'L' : '-';
		addr = (desc[0] >> SDMA_DESC0_PHY_ADDR_SHIFT)
			& SDMA_DESC0_PHY_ADDR_MASK;
		gen = (desc[1] >> SDMA_DESC1_GENERATION_SHIFT)
			& SDMA_DESC1_GENERATION_MASK;
		len = (desc[0] >> SDMA_DESC0_BYTE_COUNT_SHIFT)
			& SDMA_DESC0_BYTE_COUNT_MASK;
		dd_dev_err(sde->dd,
			   "SDMA sdmadesc[%u]: flags:%s addr:0x%016llx gen:%u len:%u bytes\n",
			   head, flags, addr, gen, len);
		dd_dev_err(sde->dd,
			   "\tdesc0:0x%016llx desc1 0x%016llx\n",
			   desc[0], desc[1]);
		if (desc[0] & SDMA_DESC0_FIRST_DESC_FLAG)
			dd_dev_err(sde->dd,
				   "\taidx: %u amode: %u alen: %u\n",
				   (u8)((desc[1] &
					 SDMA_DESC1_HEADER_INDEX_SMASK) >>
					SDMA_DESC1_HEADER_INDEX_SHIFT),
				   (u8)((desc[1] &
					 SDMA_DESC1_HEADER_MODE_SMASK) >>
					SDMA_DESC1_HEADER_MODE_SHIFT),
				   (u8)((desc[1] &
					 SDMA_DESC1_HEADER_DWS_SMASK) >>
					SDMA_DESC1_HEADER_DWS_SHIFT));
		head++;
		head &= sde->sdma_mask;
	}
}