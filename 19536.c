static int iwl_pcie_load_given_ucode(struct iwl_trans *trans,
				const struct fw_img *image)
{
	int ret = 0;
	int first_ucode_section;

	IWL_DEBUG_FW(trans, "working with %s CPU\n",
		     image->is_dual_cpus ? "Dual" : "Single");

	/* load to FW the binary non secured sections of CPU1 */
	ret = iwl_pcie_load_cpu_sections(trans, image, 1, &first_ucode_section);
	if (ret)
		return ret;

	if (image->is_dual_cpus) {
		/* set CPU2 header address */
		iwl_write_prph(trans,
			       LMPM_SECURE_UCODE_LOAD_CPU2_HDR_ADDR,
			       LMPM_SECURE_CPU2_HDR_MEM_SPACE);

		/* load to FW the binary sections of CPU2 */
		ret = iwl_pcie_load_cpu_sections(trans, image, 2,
						 &first_ucode_section);
		if (ret)
			return ret;
	}

	/* supported for 7000 only for the moment */
	if (iwlwifi_mod_params.fw_monitor &&
	    trans->trans_cfg->device_family == IWL_DEVICE_FAMILY_7000) {
		iwl_pcie_alloc_fw_monitor(trans, 0);

		if (trans->dbg.fw_mon[0].size) {
			iwl_write_prph(trans, MON_BUFF_BASE_ADDR,
				       trans->dbg.fw_mon[0].physical >> 4);
			iwl_write_prph(trans, MON_BUFF_END_ADDR,
				       (trans->dbg.fw_mon[0].physical +
					trans->dbg.fw_mon[0].size) >> 4);
		}
	} else if (iwl_pcie_dbg_on(trans)) {
		iwl_pcie_apply_destination(trans);
	}

	iwl_enable_interrupts(trans);

	/* release CPU reset */
	iwl_write32(trans, CSR_RESET, 0);

	return 0;
}