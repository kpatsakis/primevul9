static int iwl_trans_get_fw_monitor_len(struct iwl_trans *trans, u32 *len)
{
	if (trans->dbg.num_blocks) {
		*len += sizeof(struct iwl_fw_error_dump_data) +
			sizeof(struct iwl_fw_error_dump_fw_mon) +
			trans->dbg.fw_mon[0].size;
		return trans->dbg.fw_mon[0].size;
	} else if (trans->dbg.dest_tlv) {
		u32 base, end, cfg_reg, monitor_len;

		if (trans->dbg.dest_tlv->version == 1) {
			cfg_reg = le32_to_cpu(trans->dbg.dest_tlv->base_reg);
			cfg_reg = iwl_read_prph(trans, cfg_reg);
			base = (cfg_reg & IWL_LDBG_M2S_BUF_BA_MSK) <<
				trans->dbg.dest_tlv->base_shift;
			base *= IWL_M2S_UNIT_SIZE;
			base += trans->cfg->smem_offset;

			monitor_len =
				(cfg_reg & IWL_LDBG_M2S_BUF_SIZE_MSK) >>
				trans->dbg.dest_tlv->end_shift;
			monitor_len *= IWL_M2S_UNIT_SIZE;
		} else {
			base = le32_to_cpu(trans->dbg.dest_tlv->base_reg);
			end = le32_to_cpu(trans->dbg.dest_tlv->end_reg);

			base = iwl_read_prph(trans, base) <<
			       trans->dbg.dest_tlv->base_shift;
			end = iwl_read_prph(trans, end) <<
			      trans->dbg.dest_tlv->end_shift;

			/* Make "end" point to the actual end */
			if (trans->trans_cfg->device_family >=
			    IWL_DEVICE_FAMILY_8000 ||
			    trans->dbg.dest_tlv->monitor_mode == MARBH_MODE)
				end += (1 << trans->dbg.dest_tlv->end_shift);
			monitor_len = end - base;
		}
		*len += sizeof(struct iwl_fw_error_dump_data) +
			sizeof(struct iwl_fw_error_dump_fw_mon) +
			monitor_len;
		return monitor_len;
	}
	return 0;
}