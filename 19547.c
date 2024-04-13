iwl_trans_pcie_dump_monitor(struct iwl_trans *trans,
			    struct iwl_fw_error_dump_data **data,
			    u32 monitor_len)
{
	u32 len = 0;

	if (trans->dbg.dest_tlv ||
	    (trans->dbg.num_blocks &&
	     (trans->trans_cfg->device_family == IWL_DEVICE_FAMILY_7000 ||
	      trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_AX210))) {
		struct iwl_fw_error_dump_fw_mon *fw_mon_data;

		(*data)->type = cpu_to_le32(IWL_FW_ERROR_DUMP_FW_MONITOR);
		fw_mon_data = (void *)(*data)->data;

		iwl_trans_pcie_dump_pointers(trans, fw_mon_data);

		len += sizeof(**data) + sizeof(*fw_mon_data);
		if (trans->dbg.num_blocks) {
			memcpy(fw_mon_data->data,
			       trans->dbg.fw_mon[0].block,
			       trans->dbg.fw_mon[0].size);

			monitor_len = trans->dbg.fw_mon[0].size;
		} else if (trans->dbg.dest_tlv->monitor_mode == SMEM_MODE) {
			u32 base = le32_to_cpu(fw_mon_data->fw_mon_base_ptr);
			/*
			 * Update pointers to reflect actual values after
			 * shifting
			 */
			if (trans->dbg.dest_tlv->version) {
				base = (iwl_read_prph(trans, base) &
					IWL_LDBG_M2S_BUF_BA_MSK) <<
				       trans->dbg.dest_tlv->base_shift;
				base *= IWL_M2S_UNIT_SIZE;
				base += trans->cfg->smem_offset;
			} else {
				base = iwl_read_prph(trans, base) <<
				       trans->dbg.dest_tlv->base_shift;
			}

			iwl_trans_read_mem(trans, base, fw_mon_data->data,
					   monitor_len / sizeof(u32));
		} else if (trans->dbg.dest_tlv->monitor_mode == MARBH_MODE) {
			monitor_len =
				iwl_trans_pci_dump_marbh_monitor(trans,
								 fw_mon_data,
								 monitor_len);
		} else {
			/* Didn't match anything - output no monitor data */
			monitor_len = 0;
		}

		len += monitor_len;
		(*data)->len = cpu_to_le32(monitor_len + sizeof(*fw_mon_data));
	}

	return len;
}