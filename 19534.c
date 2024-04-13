iwl_trans_pcie_dump_pointers(struct iwl_trans *trans,
			     struct iwl_fw_error_dump_fw_mon *fw_mon_data)
{
	u32 base, base_high, write_ptr, write_ptr_val, wrap_cnt;

	if (trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_AX210) {
		base = DBGC_CUR_DBGBUF_BASE_ADDR_LSB;
		base_high = DBGC_CUR_DBGBUF_BASE_ADDR_MSB;
		write_ptr = DBGC_CUR_DBGBUF_STATUS;
		wrap_cnt = DBGC_DBGBUF_WRAP_AROUND;
	} else if (trans->dbg.dest_tlv) {
		write_ptr = le32_to_cpu(trans->dbg.dest_tlv->write_ptr_reg);
		wrap_cnt = le32_to_cpu(trans->dbg.dest_tlv->wrap_count);
		base = le32_to_cpu(trans->dbg.dest_tlv->base_reg);
	} else {
		base = MON_BUFF_BASE_ADDR;
		write_ptr = MON_BUFF_WRPTR;
		wrap_cnt = MON_BUFF_CYCLE_CNT;
	}

	write_ptr_val = iwl_read_prph(trans, write_ptr);
	fw_mon_data->fw_mon_cycle_cnt =
		cpu_to_le32(iwl_read_prph(trans, wrap_cnt));
	fw_mon_data->fw_mon_base_ptr =
		cpu_to_le32(iwl_read_prph(trans, base));
	if (trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_AX210) {
		fw_mon_data->fw_mon_base_high_ptr =
			cpu_to_le32(iwl_read_prph(trans, base_high));
		write_ptr_val &= DBGC_CUR_DBGBUF_STATUS_OFFSET_MSK;
	}
	fw_mon_data->fw_mon_wr_ptr = cpu_to_le32(write_ptr_val);
}