void iwl_trans_pcie_log_scd_error(struct iwl_trans *trans, struct iwl_txq *txq)
{
	u32 txq_id = txq->id;
	u32 status;
	bool active;
	u8 fifo;

	if (trans->trans_cfg->use_tfh) {
		IWL_ERR(trans, "Queue %d is stuck %d %d\n", txq_id,
			txq->read_ptr, txq->write_ptr);
		/* TODO: access new SCD registers and dump them */
		return;
	}

	status = iwl_read_prph(trans, SCD_QUEUE_STATUS_BITS(txq_id));
	fifo = (status >> SCD_QUEUE_STTS_REG_POS_TXF) & 0x7;
	active = !!(status & BIT(SCD_QUEUE_STTS_REG_POS_ACTIVE));

	IWL_ERR(trans,
		"Queue %d is %sactive on fifo %d and stuck for %u ms. SW [%d, %d] HW [%d, %d] FH TRB=0x0%x\n",
		txq_id, active ? "" : "in", fifo,
		jiffies_to_msecs(txq->wd_timeout),
		txq->read_ptr, txq->write_ptr,
		iwl_read_prph(trans, SCD_QUEUE_RDPTR(txq_id)) &
			(trans->trans_cfg->base_params->max_tfd_queue_size - 1),
			iwl_read_prph(trans, SCD_QUEUE_WRPTR(txq_id)) &
			(trans->trans_cfg->base_params->max_tfd_queue_size - 1),
			iwl_read_direct32(trans, FH_TX_TRB_REG(fifo)));
}