void bnx2x_panic_dump(struct bnx2x *bp, bool disable_int)
{
	int i;
	u16 j;
	struct hc_sp_status_block_data sp_sb_data;
	int func = BP_FUNC(bp);
#ifdef BNX2X_STOP_ON_ERROR
	u16 start = 0, end = 0;
	u8 cos;
#endif
	if (IS_PF(bp) && disable_int)
		bnx2x_int_disable(bp);

	bp->stats_state = STATS_STATE_DISABLED;
	bp->eth_stats.unrecoverable_error++;
	DP(BNX2X_MSG_STATS, "stats_state - DISABLED\n");

	BNX2X_ERR("begin crash dump -----------------\n");

	/* Indices */
	/* Common */
	if (IS_PF(bp)) {
		struct host_sp_status_block *def_sb = bp->def_status_blk;
		int data_size, cstorm_offset;

		BNX2X_ERR("def_idx(0x%x)  def_att_idx(0x%x)  attn_state(0x%x)  spq_prod_idx(0x%x) next_stats_cnt(0x%x)\n",
			  bp->def_idx, bp->def_att_idx, bp->attn_state,
			  bp->spq_prod_idx, bp->stats_counter);
		BNX2X_ERR("DSB: attn bits(0x%x)  ack(0x%x)  id(0x%x)  idx(0x%x)\n",
			  def_sb->atten_status_block.attn_bits,
			  def_sb->atten_status_block.attn_bits_ack,
			  def_sb->atten_status_block.status_block_id,
			  def_sb->atten_status_block.attn_bits_index);
		BNX2X_ERR("     def (");
		for (i = 0; i < HC_SP_SB_MAX_INDICES; i++)
			pr_cont("0x%x%s",
				def_sb->sp_sb.index_values[i],
				(i == HC_SP_SB_MAX_INDICES - 1) ? ")  " : " ");

		data_size = sizeof(struct hc_sp_status_block_data) /
			    sizeof(u32);
		cstorm_offset = CSTORM_SP_STATUS_BLOCK_DATA_OFFSET(func);
		for (i = 0; i < data_size; i++)
			*((u32 *)&sp_sb_data + i) =
				REG_RD(bp, BAR_CSTRORM_INTMEM + cstorm_offset +
					   i * sizeof(u32));

		pr_cont("igu_sb_id(0x%x)  igu_seg_id(0x%x) pf_id(0x%x)  vnic_id(0x%x)  vf_id(0x%x)  vf_valid (0x%x) state(0x%x)\n",
			sp_sb_data.igu_sb_id,
			sp_sb_data.igu_seg_id,
			sp_sb_data.p_func.pf_id,
			sp_sb_data.p_func.vnic_id,
			sp_sb_data.p_func.vf_id,
			sp_sb_data.p_func.vf_valid,
			sp_sb_data.state);
	}

	for_each_eth_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];
		int loop;
		struct hc_status_block_data_e2 sb_data_e2;
		struct hc_status_block_data_e1x sb_data_e1x;
		struct hc_status_block_sm  *hc_sm_p =
			CHIP_IS_E1x(bp) ?
			sb_data_e1x.common.state_machine :
			sb_data_e2.common.state_machine;
		struct hc_index_data *hc_index_p =
			CHIP_IS_E1x(bp) ?
			sb_data_e1x.index_data :
			sb_data_e2.index_data;
		u8 data_size, cos;
		u32 *sb_data_p;
		struct bnx2x_fp_txdata txdata;

		if (!bp->fp)
			break;

		if (!fp->rx_cons_sb)
			continue;

		/* Rx */
		BNX2X_ERR("fp%d: rx_bd_prod(0x%x)  rx_bd_cons(0x%x)  rx_comp_prod(0x%x)  rx_comp_cons(0x%x)  *rx_cons_sb(0x%x)\n",
			  i, fp->rx_bd_prod, fp->rx_bd_cons,
			  fp->rx_comp_prod,
			  fp->rx_comp_cons, le16_to_cpu(*fp->rx_cons_sb));
		BNX2X_ERR("     rx_sge_prod(0x%x)  last_max_sge(0x%x)  fp_hc_idx(0x%x)\n",
			  fp->rx_sge_prod, fp->last_max_sge,
			  le16_to_cpu(fp->fp_hc_idx));

		/* Tx */
		for_each_cos_in_tx_queue(fp, cos)
		{
			if (!fp->txdata_ptr[cos])
				break;

			txdata = *fp->txdata_ptr[cos];

			if (!txdata.tx_cons_sb)
				continue;

			BNX2X_ERR("fp%d: tx_pkt_prod(0x%x)  tx_pkt_cons(0x%x)  tx_bd_prod(0x%x)  tx_bd_cons(0x%x)  *tx_cons_sb(0x%x)\n",
				  i, txdata.tx_pkt_prod,
				  txdata.tx_pkt_cons, txdata.tx_bd_prod,
				  txdata.tx_bd_cons,
				  le16_to_cpu(*txdata.tx_cons_sb));
		}

		loop = CHIP_IS_E1x(bp) ?
			HC_SB_MAX_INDICES_E1X : HC_SB_MAX_INDICES_E2;

		/* host sb data */

		if (IS_FCOE_FP(fp))
			continue;

		BNX2X_ERR("     run indexes (");
		for (j = 0; j < HC_SB_MAX_SM; j++)
			pr_cont("0x%x%s",
			       fp->sb_running_index[j],
			       (j == HC_SB_MAX_SM - 1) ? ")" : " ");

		BNX2X_ERR("     indexes (");
		for (j = 0; j < loop; j++)
			pr_cont("0x%x%s",
			       fp->sb_index_values[j],
			       (j == loop - 1) ? ")" : " ");

		/* VF cannot access FW refelection for status block */
		if (IS_VF(bp))
			continue;

		/* fw sb data */
		data_size = CHIP_IS_E1x(bp) ?
			sizeof(struct hc_status_block_data_e1x) :
			sizeof(struct hc_status_block_data_e2);
		data_size /= sizeof(u32);
		sb_data_p = CHIP_IS_E1x(bp) ?
			(u32 *)&sb_data_e1x :
			(u32 *)&sb_data_e2;
		/* copy sb data in here */
		for (j = 0; j < data_size; j++)
			*(sb_data_p + j) = REG_RD(bp, BAR_CSTRORM_INTMEM +
				CSTORM_STATUS_BLOCK_DATA_OFFSET(fp->fw_sb_id) +
				j * sizeof(u32));

		if (!CHIP_IS_E1x(bp)) {
			pr_cont("pf_id(0x%x)  vf_id(0x%x)  vf_valid(0x%x) vnic_id(0x%x)  same_igu_sb_1b(0x%x) state(0x%x)\n",
				sb_data_e2.common.p_func.pf_id,
				sb_data_e2.common.p_func.vf_id,
				sb_data_e2.common.p_func.vf_valid,
				sb_data_e2.common.p_func.vnic_id,
				sb_data_e2.common.same_igu_sb_1b,
				sb_data_e2.common.state);
		} else {
			pr_cont("pf_id(0x%x)  vf_id(0x%x)  vf_valid(0x%x) vnic_id(0x%x)  same_igu_sb_1b(0x%x) state(0x%x)\n",
				sb_data_e1x.common.p_func.pf_id,
				sb_data_e1x.common.p_func.vf_id,
				sb_data_e1x.common.p_func.vf_valid,
				sb_data_e1x.common.p_func.vnic_id,
				sb_data_e1x.common.same_igu_sb_1b,
				sb_data_e1x.common.state);
		}

		/* SB_SMs data */
		for (j = 0; j < HC_SB_MAX_SM; j++) {
			pr_cont("SM[%d] __flags (0x%x) igu_sb_id (0x%x)  igu_seg_id(0x%x) time_to_expire (0x%x) timer_value(0x%x)\n",
				j, hc_sm_p[j].__flags,
				hc_sm_p[j].igu_sb_id,
				hc_sm_p[j].igu_seg_id,
				hc_sm_p[j].time_to_expire,
				hc_sm_p[j].timer_value);
		}

		/* Indices data */
		for (j = 0; j < loop; j++) {
			pr_cont("INDEX[%d] flags (0x%x) timeout (0x%x)\n", j,
			       hc_index_p[j].flags,
			       hc_index_p[j].timeout);
		}
	}

#ifdef BNX2X_STOP_ON_ERROR
	if (IS_PF(bp)) {
		/* event queue */
		BNX2X_ERR("eq cons %x prod %x\n", bp->eq_cons, bp->eq_prod);
		for (i = 0; i < NUM_EQ_DESC; i++) {
			u32 *data = (u32 *)&bp->eq_ring[i].message.data;

			BNX2X_ERR("event queue [%d]: header: opcode %d, error %d\n",
				  i, bp->eq_ring[i].message.opcode,
				  bp->eq_ring[i].message.error);
			BNX2X_ERR("data: %x %x %x\n",
				  data[0], data[1], data[2]);
		}
	}

	/* Rings */
	/* Rx */
	for_each_valid_rx_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];

		if (!bp->fp)
			break;

		if (!fp->rx_cons_sb)
			continue;

		start = RX_BD(le16_to_cpu(*fp->rx_cons_sb) - 10);
		end = RX_BD(le16_to_cpu(*fp->rx_cons_sb) + 503);
		for (j = start; j != end; j = RX_BD(j + 1)) {
			u32 *rx_bd = (u32 *)&fp->rx_desc_ring[j];
			struct sw_rx_bd *sw_bd = &fp->rx_buf_ring[j];

			BNX2X_ERR("fp%d: rx_bd[%x]=[%x:%x]  sw_bd=[%p]\n",
				  i, j, rx_bd[1], rx_bd[0], sw_bd->data);
		}

		start = RX_SGE(fp->rx_sge_prod);
		end = RX_SGE(fp->last_max_sge);
		for (j = start; j != end; j = RX_SGE(j + 1)) {
			u32 *rx_sge = (u32 *)&fp->rx_sge_ring[j];
			struct sw_rx_page *sw_page = &fp->rx_page_ring[j];

			BNX2X_ERR("fp%d: rx_sge[%x]=[%x:%x]  sw_page=[%p]\n",
				  i, j, rx_sge[1], rx_sge[0], sw_page->page);
		}

		start = RCQ_BD(fp->rx_comp_cons - 10);
		end = RCQ_BD(fp->rx_comp_cons + 503);
		for (j = start; j != end; j = RCQ_BD(j + 1)) {
			u32 *cqe = (u32 *)&fp->rx_comp_ring[j];

			BNX2X_ERR("fp%d: cqe[%x]=[%x:%x:%x:%x]\n",
				  i, j, cqe[0], cqe[1], cqe[2], cqe[3]);
		}
	}

	/* Tx */
	for_each_valid_tx_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];

		if (!bp->fp)
			break;

		for_each_cos_in_tx_queue(fp, cos) {
			struct bnx2x_fp_txdata *txdata = fp->txdata_ptr[cos];

			if (!fp->txdata_ptr[cos])
				break;

			if (!txdata->tx_cons_sb)
				continue;

			start = TX_BD(le16_to_cpu(*txdata->tx_cons_sb) - 10);
			end = TX_BD(le16_to_cpu(*txdata->tx_cons_sb) + 245);
			for (j = start; j != end; j = TX_BD(j + 1)) {
				struct sw_tx_bd *sw_bd =
					&txdata->tx_buf_ring[j];

				BNX2X_ERR("fp%d: txdata %d, packet[%x]=[%p,%x]\n",
					  i, cos, j, sw_bd->skb,
					  sw_bd->first_bd);
			}

			start = TX_BD(txdata->tx_bd_cons - 10);
			end = TX_BD(txdata->tx_bd_cons + 254);
			for (j = start; j != end; j = TX_BD(j + 1)) {
				u32 *tx_bd = (u32 *)&txdata->tx_desc_ring[j];

				BNX2X_ERR("fp%d: txdata %d, tx_bd[%x]=[%x:%x:%x:%x]\n",
					  i, cos, j, tx_bd[0], tx_bd[1],
					  tx_bd[2], tx_bd[3]);
			}
		}
	}
#endif
	if (IS_PF(bp)) {
		bnx2x_fw_dump(bp);
		bnx2x_mc_assert(bp);
	}
	BNX2X_ERR("end crash dump -----------------\n");
}