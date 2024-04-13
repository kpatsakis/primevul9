void iwl_pcie_apply_destination(struct iwl_trans *trans)
{
	const struct iwl_fw_dbg_dest_tlv_v1 *dest = trans->dbg.dest_tlv;
	int i;

	if (iwl_trans_dbg_ini_valid(trans)) {
		if (!trans->dbg.num_blocks)
			return;

		IWL_DEBUG_FW(trans,
			     "WRT: Applying DRAM buffer[0] destination\n");
		iwl_write_umac_prph(trans, MON_BUFF_BASE_ADDR_VER2,
				    trans->dbg.fw_mon[0].physical >>
				    MON_BUFF_SHIFT_VER2);
		iwl_write_umac_prph(trans, MON_BUFF_END_ADDR_VER2,
				    (trans->dbg.fw_mon[0].physical +
				     trans->dbg.fw_mon[0].size - 256) >>
				    MON_BUFF_SHIFT_VER2);
		return;
	}

	IWL_INFO(trans, "Applying debug destination %s\n",
		 get_fw_dbg_mode_string(dest->monitor_mode));

	if (dest->monitor_mode == EXTERNAL_MODE)
		iwl_pcie_alloc_fw_monitor(trans, dest->size_power);
	else
		IWL_WARN(trans, "PCI should have external buffer debug\n");

	for (i = 0; i < trans->dbg.n_dest_reg; i++) {
		u32 addr = le32_to_cpu(dest->reg_ops[i].addr);
		u32 val = le32_to_cpu(dest->reg_ops[i].val);

		switch (dest->reg_ops[i].op) {
		case CSR_ASSIGN:
			iwl_write32(trans, addr, val);
			break;
		case CSR_SETBIT:
			iwl_set_bit(trans, addr, BIT(val));
			break;
		case CSR_CLEARBIT:
			iwl_clear_bit(trans, addr, BIT(val));
			break;
		case PRPH_ASSIGN:
			iwl_write_prph(trans, addr, val);
			break;
		case PRPH_SETBIT:
			iwl_set_bits_prph(trans, addr, BIT(val));
			break;
		case PRPH_CLEARBIT:
			iwl_clear_bits_prph(trans, addr, BIT(val));
			break;
		case PRPH_BLOCKBIT:
			if (iwl_read_prph(trans, addr) & BIT(val)) {
				IWL_ERR(trans,
					"BIT(%u) in address 0x%x is 1, stopping FW configuration\n",
					val, addr);
				goto monitor;
			}
			break;
		default:
			IWL_ERR(trans, "FW debug - unknown OP %d\n",
				dest->reg_ops[i].op);
			break;
		}
	}

monitor:
	if (dest->monitor_mode == EXTERNAL_MODE && trans->dbg.fw_mon[0].size) {
		iwl_write_prph(trans, le32_to_cpu(dest->base_reg),
			       trans->dbg.fw_mon[0].physical >>
			       dest->base_shift);
		if (trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_8000)
			iwl_write_prph(trans, le32_to_cpu(dest->end_reg),
				       (trans->dbg.fw_mon[0].physical +
					trans->dbg.fw_mon[0].size - 256) >>
						dest->end_shift);
		else
			iwl_write_prph(trans, le32_to_cpu(dest->end_reg),
				       (trans->dbg.fw_mon[0].physical +
					trans->dbg.fw_mon[0].size) >>
						dest->end_shift);
	}
}