static ssize_t iwl_dbgfs_tx_queue_read(struct file *file,
				       char __user *user_buf,
				       size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_txq *txq;
	char *buf;
	int pos = 0;
	int cnt;
	int ret;
	size_t bufsz;

	bufsz = sizeof(char) * 75 *
		trans->trans_cfg->base_params->num_of_queues;

	if (!trans_pcie->txq_memory)
		return -EAGAIN;

	buf = kzalloc(bufsz, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	for (cnt = 0;
	     cnt < trans->trans_cfg->base_params->num_of_queues;
	     cnt++) {
		txq = trans_pcie->txq[cnt];
		pos += scnprintf(buf + pos, bufsz - pos,
				"hwq %.2d: read=%u write=%u use=%d stop=%d need_update=%d frozen=%d%s\n",
				cnt, txq->read_ptr, txq->write_ptr,
				!!test_bit(cnt, trans_pcie->queue_used),
				 !!test_bit(cnt, trans_pcie->queue_stopped),
				 txq->need_update, txq->frozen,
				 (cnt == trans_pcie->cmd_queue ? " HCMD" : ""));
	}
	ret = simple_read_from_buffer(user_buf, count, ppos, buf, pos);
	kfree(buf);
	return ret;
}