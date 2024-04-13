static void bnx2x_after_afex_vif_lists(struct bnx2x *bp,
					      union event_ring_elem *elem)
{
	if (elem->message.data.vif_list_event.echo == VIF_LIST_RULE_GET) {
		DP(BNX2X_MSG_SP,
		   "afex: ramrod completed VIF LIST_GET, addrs 0x%x\n",
		   elem->message.data.vif_list_event.func_bit_map);
		bnx2x_fw_command(bp, DRV_MSG_CODE_AFEX_LISTGET_ACK,
			elem->message.data.vif_list_event.func_bit_map);
	} else if (elem->message.data.vif_list_event.echo ==
		   VIF_LIST_RULE_SET) {
		DP(BNX2X_MSG_SP, "afex: ramrod completed VIF LIST_SET\n");
		bnx2x_fw_command(bp, DRV_MSG_CODE_AFEX_LISTSET_ACK, 0);
	}
}