static ssize_t iwl_dbgfs_rx_queue_read(struct file *file,
				       char __user *user_buf,
				       size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	char *buf;
	int pos = 0, i, ret;
	size_t bufsz = sizeof(buf);

	bufsz = sizeof(char) * 121 * trans->num_rx_queues;

	if (!trans_pcie->rxq)
		return -EAGAIN;

	buf = kzalloc(bufsz, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	for (i = 0; i < trans->num_rx_queues && pos < bufsz; i++) {
		struct iwl_rxq *rxq = &trans_pcie->rxq[i];

		pos += scnprintf(buf + pos, bufsz - pos, "queue#: %2d\n",
				 i);
		pos += scnprintf(buf + pos, bufsz - pos, "\tread: %u\n",
				 rxq->read);
		pos += scnprintf(buf + pos, bufsz - pos, "\twrite: %u\n",
				 rxq->write);
		pos += scnprintf(buf + pos, bufsz - pos, "\twrite_actual: %u\n",
				 rxq->write_actual);
		pos += scnprintf(buf + pos, bufsz - pos, "\tneed_update: %2d\n",
				 rxq->need_update);
		pos += scnprintf(buf + pos, bufsz - pos, "\tfree_count: %u\n",
				 rxq->free_count);
		if (rxq->rb_stts) {
			u32 r =	__le16_to_cpu(iwl_get_closed_rb_stts(trans,
								     rxq));
			pos += scnprintf(buf + pos, bufsz - pos,
					 "\tclosed_rb_num: %u\n",
					 r & 0x0FFF);
		} else {
			pos += scnprintf(buf + pos, bufsz - pos,
					 "\tclosed_rb_num: Not Allocated\n");
		}
	}
	ret = simple_read_from_buffer(user_buf, count, ppos, buf, pos);
	kfree(buf);

	return ret;
}