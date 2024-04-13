static int iwl_trans_pcie_clear_persistence_bit(struct iwl_trans *trans)
{
	u32 hpm, wprot;

	switch (trans->trans_cfg->device_family) {
	case IWL_DEVICE_FAMILY_9000:
		wprot = PREG_PRPH_WPROT_9000;
		break;
	case IWL_DEVICE_FAMILY_22000:
		wprot = PREG_PRPH_WPROT_22000;
		break;
	default:
		return 0;
	}

	hpm = iwl_read_umac_prph_no_grab(trans, HPM_DEBUG);
	if (hpm != 0xa5a5a5a0 && (hpm & PERSISTENCE_BIT)) {
		u32 wprot_val = iwl_read_umac_prph_no_grab(trans, wprot);

		if (wprot_val & PREG_WFPM_ACCESS) {
			IWL_ERR(trans,
				"Error, can not clear persistence bit\n");
			return -EPERM;
		}
		iwl_write_umac_prph_no_grab(trans, HPM_DEBUG,
					    hpm & ~PERSISTENCE_BIT);
	}

	return 0;
}