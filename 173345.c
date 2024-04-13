static void bnx2x_pf_rx_q_prep(struct bnx2x *bp,
	struct bnx2x_fastpath *fp, struct rxq_pause_params *pause,
	struct bnx2x_rxq_setup_params *rxq_init)
{
	u8 max_sge = 0;
	u16 sge_sz = 0;
	u16 tpa_agg_size = 0;

	if (fp->mode != TPA_MODE_DISABLED) {
		pause->sge_th_lo = SGE_TH_LO(bp);
		pause->sge_th_hi = SGE_TH_HI(bp);

		/* validate SGE ring has enough to cross high threshold */
		WARN_ON(bp->dropless_fc &&
				pause->sge_th_hi + FW_PREFETCH_CNT >
				MAX_RX_SGE_CNT * NUM_RX_SGE_PAGES);

		tpa_agg_size = TPA_AGG_SIZE;
		max_sge = SGE_PAGE_ALIGN(bp->dev->mtu) >>
			SGE_PAGE_SHIFT;
		max_sge = ((max_sge + PAGES_PER_SGE - 1) &
			  (~(PAGES_PER_SGE-1))) >> PAGES_PER_SGE_SHIFT;
		sge_sz = (u16)min_t(u32, SGE_PAGES, 0xffff);
	}

	/* pause - not for e1 */
	if (!CHIP_IS_E1(bp)) {
		pause->bd_th_lo = BD_TH_LO(bp);
		pause->bd_th_hi = BD_TH_HI(bp);

		pause->rcq_th_lo = RCQ_TH_LO(bp);
		pause->rcq_th_hi = RCQ_TH_HI(bp);
		/*
		 * validate that rings have enough entries to cross
		 * high thresholds
		 */
		WARN_ON(bp->dropless_fc &&
				pause->bd_th_hi + FW_PREFETCH_CNT >
				bp->rx_ring_size);
		WARN_ON(bp->dropless_fc &&
				pause->rcq_th_hi + FW_PREFETCH_CNT >
				NUM_RCQ_RINGS * MAX_RCQ_DESC_CNT);

		pause->pri_map = 1;
	}

	/* rxq setup */
	rxq_init->dscr_map = fp->rx_desc_mapping;
	rxq_init->sge_map = fp->rx_sge_mapping;
	rxq_init->rcq_map = fp->rx_comp_mapping;
	rxq_init->rcq_np_map = fp->rx_comp_mapping + BCM_PAGE_SIZE;

	/* This should be a maximum number of data bytes that may be
	 * placed on the BD (not including paddings).
	 */
	rxq_init->buf_sz = fp->rx_buf_size - BNX2X_FW_RX_ALIGN_START -
			   BNX2X_FW_RX_ALIGN_END - IP_HEADER_ALIGNMENT_PADDING;

	rxq_init->cl_qzone_id = fp->cl_qzone_id;
	rxq_init->tpa_agg_sz = tpa_agg_size;
	rxq_init->sge_buf_sz = sge_sz;
	rxq_init->max_sges_pkt = max_sge;
	rxq_init->rss_engine_id = BP_FUNC(bp);
	rxq_init->mcast_engine_id = BP_FUNC(bp);

	/* Maximum number or simultaneous TPA aggregation for this Queue.
	 *
	 * For PF Clients it should be the maximum available number.
	 * VF driver(s) may want to define it to a smaller value.
	 */
	rxq_init->max_tpa_queues = MAX_AGG_QS(bp);

	rxq_init->cache_line_log = BNX2X_RX_ALIGN_SHIFT;
	rxq_init->fw_sb_id = fp->fw_sb_id;

	if (IS_FCOE_FP(fp))
		rxq_init->sb_cq_index = HC_SP_INDEX_ETH_FCOE_RX_CQ_CONS;
	else
		rxq_init->sb_cq_index = HC_INDEX_ETH_RX_CQ_CONS;
	/* configure silent vlan removal
	 * if multi function mode is afex, then mask default vlan
	 */
	if (IS_MF_AFEX(bp)) {
		rxq_init->silent_removal_value = bp->afex_def_vlan_tag;
		rxq_init->silent_removal_mask = VLAN_VID_MASK;
	}
}