static int i40e_init_pf_dcb(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	int err = 0;

	/* Do not enable DCB for SW1 and SW2 images even if the FW is capable
	 * Also do not enable DCBx if FW LLDP agent is disabled
	 */
	if ((pf->hw_features & I40E_HW_NO_DCB_SUPPORT) ||
	    (pf->flags & I40E_FLAG_DISABLE_FW_LLDP)) {
		dev_info(&pf->pdev->dev, "DCB is not supported or FW LLDP is disabled\n");
		err = I40E_NOT_SUPPORTED;
		goto out;
	}

	err = i40e_init_dcb(hw, true);
	if (!err) {
		/* Device/Function is not DCBX capable */
		if ((!hw->func_caps.dcb) ||
		    (hw->dcbx_status == I40E_DCBX_STATUS_DISABLED)) {
			dev_info(&pf->pdev->dev,
				 "DCBX offload is not supported or is disabled for this PF.\n");
		} else {
			/* When status is not DISABLED then DCBX in FW */
			pf->dcbx_cap = DCB_CAP_DCBX_LLD_MANAGED |
				       DCB_CAP_DCBX_VER_IEEE;

			pf->flags |= I40E_FLAG_DCB_CAPABLE;
			/* Enable DCB tagging only when more than one TC
			 * or explicitly disable if only one TC
			 */
			if (i40e_dcb_get_num_tc(&hw->local_dcbx_config) > 1)
				pf->flags |= I40E_FLAG_DCB_ENABLED;
			else
				pf->flags &= ~I40E_FLAG_DCB_ENABLED;
			dev_dbg(&pf->pdev->dev,
				"DCBX offload is supported for this PF.\n");
		}
	} else if (pf->hw.aq.asq_last_status == I40E_AQ_RC_EPERM) {
		dev_info(&pf->pdev->dev, "FW LLDP disabled for this PF.\n");
		pf->flags |= I40E_FLAG_DISABLE_FW_LLDP;
	} else {
		dev_info(&pf->pdev->dev,
			 "Query for DCB configuration failed, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, err),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
	}

out:
	return err;
}