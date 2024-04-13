static ssize_t iwl_dbgfs_monitor_data_read(struct file *file,
					   char __user *user_buf,
					   size_t count, loff_t *ppos)
{
	struct iwl_trans *trans = file->private_data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	void *cpu_addr = (void *)trans->dbg.fw_mon[0].block, *curr_buf;
	struct cont_rec *data = &trans_pcie->fw_mon_data;
	u32 write_ptr_addr, wrap_cnt_addr, write_ptr, wrap_cnt;
	ssize_t size, bytes_copied = 0;
	bool b_full;

	if (trans->dbg.dest_tlv) {
		write_ptr_addr =
			le32_to_cpu(trans->dbg.dest_tlv->write_ptr_reg);
		wrap_cnt_addr = le32_to_cpu(trans->dbg.dest_tlv->wrap_count);
	} else {
		write_ptr_addr = MON_BUFF_WRPTR;
		wrap_cnt_addr = MON_BUFF_CYCLE_CNT;
	}

	if (unlikely(!trans->dbg.rec_on))
		return 0;

	mutex_lock(&data->mutex);
	if (data->state ==
	    IWL_FW_MON_DBGFS_STATE_DISABLED) {
		mutex_unlock(&data->mutex);
		return 0;
	}

	/* write_ptr position in bytes rather then DW */
	write_ptr = iwl_read_prph(trans, write_ptr_addr) * sizeof(u32);
	wrap_cnt = iwl_read_prph(trans, wrap_cnt_addr);

	if (data->prev_wrap_cnt == wrap_cnt) {
		size = write_ptr - data->prev_wr_ptr;
		curr_buf = cpu_addr + data->prev_wr_ptr;
		b_full = iwl_write_to_user_buf(user_buf, count,
					       curr_buf, &size,
					       &bytes_copied);
		data->prev_wr_ptr += size;

	} else if (data->prev_wrap_cnt == wrap_cnt - 1 &&
		   write_ptr < data->prev_wr_ptr) {
		size = trans->dbg.fw_mon[0].size - data->prev_wr_ptr;
		curr_buf = cpu_addr + data->prev_wr_ptr;
		b_full = iwl_write_to_user_buf(user_buf, count,
					       curr_buf, &size,
					       &bytes_copied);
		data->prev_wr_ptr += size;

		if (!b_full) {
			size = write_ptr;
			b_full = iwl_write_to_user_buf(user_buf, count,
						       cpu_addr, &size,
						       &bytes_copied);
			data->prev_wr_ptr = size;
			data->prev_wrap_cnt++;
		}
	} else {
		if (data->prev_wrap_cnt == wrap_cnt - 1 &&
		    write_ptr > data->prev_wr_ptr)
			IWL_WARN(trans,
				 "write pointer passed previous write pointer, start copying from the beginning\n");
		else if (!unlikely(data->prev_wrap_cnt == 0 &&
				   data->prev_wr_ptr == 0))
			IWL_WARN(trans,
				 "monitor data is out of sync, start copying from the beginning\n");

		size = write_ptr;
		b_full = iwl_write_to_user_buf(user_buf, count,
					       cpu_addr, &size,
					       &bytes_copied);
		data->prev_wr_ptr = size;
		data->prev_wrap_cnt = wrap_cnt;
	}

	mutex_unlock(&data->mutex);

	return bytes_copied;
}