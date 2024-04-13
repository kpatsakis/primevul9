void sdma_seqfile_dump_sde(struct seq_file *s, struct sdma_engine *sde)
{
	u16 head, tail;
	struct hw_sdma_desc *descqp;
	u64 desc[2];
	u64 addr;
	u8 gen;
	u16 len;

	head = sde->descq_head & sde->sdma_mask;
	tail = READ_ONCE(sde->descq_tail) & sde->sdma_mask;
	seq_printf(s, SDE_FMT, sde->this_idx,
		   sde->cpu,
		   sdma_state_name(sde->state.current_state),
		   (unsigned long long)read_sde_csr(sde, SD(CTRL)),
		   (unsigned long long)read_sde_csr(sde, SD(STATUS)),
		   (unsigned long long)read_sde_csr(sde, SD(ENG_ERR_STATUS)),
		   (unsigned long long)read_sde_csr(sde, SD(TAIL)), tail,
		   (unsigned long long)read_sde_csr(sde, SD(HEAD)), head,
		   (unsigned long long)le64_to_cpu(*sde->head_dma),
		   (unsigned long long)read_sde_csr(sde, SD(MEMORY)),
		   (unsigned long long)read_sde_csr(sde, SD(LEN_GEN)),
		   (unsigned long long)read_sde_csr(sde, SD(RELOAD_CNT)),
		   (unsigned long long)sde->last_status,
		   (unsigned long long)sde->ahg_bits,
		   sde->tx_tail,
		   sde->tx_head,
		   sde->descq_tail,
		   sde->descq_head,
		   !list_empty(&sde->flushlist),
		   sde->descq_full_count,
		   (unsigned long long)read_sde_csr(sde, SEND_DMA_CHECK_SLID));

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
		seq_printf(s,
			   "\tdesc[%u]: flags:%s addr:0x%016llx gen:%u len:%u bytes\n",
			   head, flags, addr, gen, len);
		if (desc[0] & SDMA_DESC0_FIRST_DESC_FLAG)
			seq_printf(s, "\t\tahgidx: %u ahgmode: %u\n",
				   (u8)((desc[1] &
					 SDMA_DESC1_HEADER_INDEX_SMASK) >>
					SDMA_DESC1_HEADER_INDEX_SHIFT),
				   (u8)((desc[1] &
					 SDMA_DESC1_HEADER_MODE_SMASK) >>
					SDMA_DESC1_HEADER_MODE_SHIFT));
		head = (head + 1) & sde->sdma_mask;
	}
}