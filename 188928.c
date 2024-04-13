static u8 i40e_get_iscsi_tc_map(struct i40e_pf *pf)
{
	struct i40e_dcb_app_priority_table app;
	struct i40e_hw *hw = &pf->hw;
	u8 enabled_tc = 1; /* TC0 is always enabled */
	u8 tc, i;
	/* Get the iSCSI APP TLV */
	struct i40e_dcbx_config *dcbcfg = &hw->local_dcbx_config;

	for (i = 0; i < dcbcfg->numapps; i++) {
		app = dcbcfg->app[i];
		if (app.selector == I40E_APP_SEL_TCPIP &&
		    app.protocolid == I40E_APP_PROTOID_ISCSI) {
			tc = dcbcfg->etscfg.prioritytable[app.priority];
			enabled_tc |= BIT(tc);
			break;
		}
	}

	return enabled_tc;
}