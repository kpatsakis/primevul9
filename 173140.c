static int bnx2x_afex_func_update(struct bnx2x *bp, u16 vifid,
				  u16 vlan_val, u8 allowed_prio)
{
	struct bnx2x_func_state_params func_params = {NULL};
	struct bnx2x_func_afex_update_params *f_update_params =
		&func_params.params.afex_update;

	func_params.f_obj = &bp->func_obj;
	func_params.cmd = BNX2X_F_CMD_AFEX_UPDATE;

	/* no need to wait for RAMROD completion, so don't
	 * set RAMROD_COMP_WAIT flag
	 */

	f_update_params->vif_id = vifid;
	f_update_params->afex_default_vlan = vlan_val;
	f_update_params->allowed_priorities = allowed_prio;

	/* if ramrod can not be sent, response to MCP immediately */
	if (bnx2x_func_state_change(bp, &func_params) < 0)
		bnx2x_fw_command(bp, DRV_MSG_CODE_AFEX_VIFSET_ACK, 0);

	return 0;
}