static bool i40e_check_recovery_mode(struct i40e_pf *pf)
{
	u32 val = rd32(&pf->hw, I40E_GL_FWSTS) & I40E_GL_FWSTS_FWS1B_MASK;
	bool is_recovery_mode = false;

	if (pf->hw.mac.type == I40E_MAC_XL710)
		is_recovery_mode =
		val == I40E_XL710_GL_FWSTS_FWS1B_REC_MOD_CORER_MASK ||
		val == I40E_XL710_GL_FWSTS_FWS1B_REC_MOD_GLOBR_MASK ||
		val == I40E_XL710_GL_FWSTS_FWS1B_REC_MOD_TRANSITION_MASK ||
		val == I40E_XL710_GL_FWSTS_FWS1B_REC_MOD_NVM_MASK;
	if (pf->hw.mac.type == I40E_MAC_X722)
		is_recovery_mode =
		val == I40E_X722_GL_FWSTS_FWS1B_REC_MOD_CORER_MASK ||
		val == I40E_X722_GL_FWSTS_FWS1B_REC_MOD_GLOBR_MASK;
	if (is_recovery_mode) {
		dev_notice(&pf->pdev->dev, "Firmware recovery mode detected. Limiting functionality.\n");
		dev_notice(&pf->pdev->dev, "Refer to the Intel(R) Ethernet Adapters and Devices User Guide for details on firmware recovery mode.\n");
		set_bit(__I40E_RECOVERY_MODE, pf->state);

		return true;
	}
	if (test_and_clear_bit(__I40E_RECOVERY_MODE, pf->state))
		dev_info(&pf->pdev->dev, "Reinitializing in normal mode with full functionality.\n");

	return false;
}