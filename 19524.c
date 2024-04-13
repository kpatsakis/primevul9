*iwl_trans_pcie_dump_data(struct iwl_trans *trans,
			  u32 dump_mask)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_fw_error_dump_data *data;
	struct iwl_txq *cmdq = trans_pcie->txq[trans_pcie->cmd_queue];
	struct iwl_fw_error_dump_txcmd *txcmd;
	struct iwl_trans_dump_data *dump_data;
	u32 len, num_rbs = 0, monitor_len = 0;
	int i, ptr;
	bool dump_rbs = test_bit(STATUS_FW_ERROR, &trans->status) &&
			!trans->trans_cfg->mq_rx_supported &&
			dump_mask & BIT(IWL_FW_ERROR_DUMP_RB);

	if (!dump_mask)
		return NULL;

	/* transport dump header */
	len = sizeof(*dump_data);

	/* host commands */
	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_TXCMD) && cmdq)
		len += sizeof(*data) +
			cmdq->n_window * (sizeof(*txcmd) +
					  TFD_MAX_PAYLOAD_SIZE);

	/* FW monitor */
	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_FW_MONITOR))
		monitor_len = iwl_trans_get_fw_monitor_len(trans, &len);

	/* CSR registers */
	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_CSR))
		len += sizeof(*data) + IWL_CSR_TO_DUMP;

	/* FH registers */
	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_FH_REGS)) {
		if (trans->trans_cfg->gen2)
			len += sizeof(*data) +
			       (iwl_umac_prph(trans, FH_MEM_UPPER_BOUND_GEN2) -
				iwl_umac_prph(trans, FH_MEM_LOWER_BOUND_GEN2));
		else
			len += sizeof(*data) +
			       (FH_MEM_UPPER_BOUND -
				FH_MEM_LOWER_BOUND);
	}

	if (dump_rbs) {
		/* Dump RBs is supported only for pre-9000 devices (1 queue) */
		struct iwl_rxq *rxq = &trans_pcie->rxq[0];
		/* RBs */
		num_rbs =
			le16_to_cpu(iwl_get_closed_rb_stts(trans, rxq))
			& 0x0FFF;
		num_rbs = (num_rbs - rxq->read) & RX_QUEUE_MASK;
		len += num_rbs * (sizeof(*data) +
				  sizeof(struct iwl_fw_error_dump_rb) +
				  (PAGE_SIZE << trans_pcie->rx_page_order));
	}

	/* Paged memory for gen2 HW */
	if (trans->trans_cfg->gen2 && dump_mask & BIT(IWL_FW_ERROR_DUMP_PAGING))
		for (i = 0; i < trans->init_dram.paging_cnt; i++)
			len += sizeof(*data) +
			       sizeof(struct iwl_fw_error_dump_paging) +
			       trans->init_dram.paging[i].size;

	dump_data = vzalloc(len);
	if (!dump_data)
		return NULL;

	len = 0;
	data = (void *)dump_data->data;

	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_TXCMD) && cmdq) {
		u16 tfd_size = trans_pcie->tfd_size;

		data->type = cpu_to_le32(IWL_FW_ERROR_DUMP_TXCMD);
		txcmd = (void *)data->data;
		spin_lock_bh(&cmdq->lock);
		ptr = cmdq->write_ptr;
		for (i = 0; i < cmdq->n_window; i++) {
			u8 idx = iwl_pcie_get_cmd_index(cmdq, ptr);
			u8 tfdidx;
			u32 caplen, cmdlen;

			if (trans->trans_cfg->use_tfh)
				tfdidx = idx;
			else
				tfdidx = ptr;

			cmdlen = iwl_trans_pcie_get_cmdlen(trans,
							   (u8 *)cmdq->tfds +
							   tfd_size * tfdidx);
			caplen = min_t(u32, TFD_MAX_PAYLOAD_SIZE, cmdlen);

			if (cmdlen) {
				len += sizeof(*txcmd) + caplen;
				txcmd->cmdlen = cpu_to_le32(cmdlen);
				txcmd->caplen = cpu_to_le32(caplen);
				memcpy(txcmd->data, cmdq->entries[idx].cmd,
				       caplen);
				txcmd = (void *)((u8 *)txcmd->data + caplen);
			}

			ptr = iwl_queue_dec_wrap(trans, ptr);
		}
		spin_unlock_bh(&cmdq->lock);

		data->len = cpu_to_le32(len);
		len += sizeof(*data);
		data = iwl_fw_error_next_data(data);
	}

	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_CSR))
		len += iwl_trans_pcie_dump_csr(trans, &data);
	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_FH_REGS))
		len += iwl_trans_pcie_fh_regs_dump(trans, &data);
	if (dump_rbs)
		len += iwl_trans_pcie_dump_rbs(trans, &data, num_rbs);

	/* Paged memory for gen2 HW */
	if (trans->trans_cfg->gen2 &&
	    dump_mask & BIT(IWL_FW_ERROR_DUMP_PAGING)) {
		for (i = 0; i < trans->init_dram.paging_cnt; i++) {
			struct iwl_fw_error_dump_paging *paging;
			u32 page_len = trans->init_dram.paging[i].size;

			data->type = cpu_to_le32(IWL_FW_ERROR_DUMP_PAGING);
			data->len = cpu_to_le32(sizeof(*paging) + page_len);
			paging = (void *)data->data;
			paging->index = cpu_to_le32(i);
			memcpy(paging->data,
			       trans->init_dram.paging[i].block, page_len);
			data = iwl_fw_error_next_data(data);

			len += sizeof(*data) + sizeof(*paging) + page_len;
		}
	}
	if (dump_mask & BIT(IWL_FW_ERROR_DUMP_FW_MONITOR))
		len += iwl_trans_pcie_dump_monitor(trans, &data, monitor_len);

	dump_data->len = len;

	return dump_data;
}