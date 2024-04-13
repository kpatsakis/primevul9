static void i40e_config_bridge_mode(struct i40e_veb *veb)
{
	struct i40e_pf *pf = veb->pf;

	if (pf->hw.debug_mask & I40E_DEBUG_LAN)
		dev_info(&pf->pdev->dev, "enabling bridge mode: %s\n",
			 veb->bridge_mode == BRIDGE_MODE_VEPA ? "VEPA" : "VEB");
	if (veb->bridge_mode & BRIDGE_MODE_VEPA)
		i40e_disable_pf_switch_lb(pf);
	else
		i40e_enable_pf_switch_lb(pf);
}