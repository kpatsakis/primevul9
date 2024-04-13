static u32 iwl_trans_pcie_fh_regs_dump(struct iwl_trans *trans,
				       struct iwl_fw_error_dump_data **data)
{
	u32 fh_regs_len = FH_MEM_UPPER_BOUND - FH_MEM_LOWER_BOUND;
	unsigned long flags;
	__le32 *val;
	int i;

	if (!iwl_trans_grab_nic_access(trans, &flags))
		return 0;

	(*data)->type = cpu_to_le32(IWL_FW_ERROR_DUMP_FH_REGS);
	(*data)->len = cpu_to_le32(fh_regs_len);
	val = (void *)(*data)->data;

	if (!trans->trans_cfg->gen2)
		for (i = FH_MEM_LOWER_BOUND; i < FH_MEM_UPPER_BOUND;
		     i += sizeof(u32))
			*val++ = cpu_to_le32(iwl_trans_pcie_read32(trans, i));
	else
		for (i = iwl_umac_prph(trans, FH_MEM_LOWER_BOUND_GEN2);
		     i < iwl_umac_prph(trans, FH_MEM_UPPER_BOUND_GEN2);
		     i += sizeof(u32))
			*val++ = cpu_to_le32(iwl_trans_pcie_read_prph(trans,
								      i));

	iwl_trans_release_nic_access(trans, &flags);

	*data = iwl_fw_error_next_data(*data);

	return sizeof(**data) + fh_regs_len;
}