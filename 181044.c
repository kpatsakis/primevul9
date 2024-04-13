int ath6kl_wmi_pmparams_cmd(struct wmi *wmi, u8 if_idx, u16 idle_period,
			    u16 ps_poll_num, u16 dtim_policy,
			    u16 tx_wakeup_policy, u16 num_tx_to_wakeup,
			    u16 ps_fail_event_policy)
{
	struct sk_buff *skb;
	struct wmi_power_params_cmd *pm;
	int ret;

	skb = ath6kl_wmi_get_new_buf(sizeof(*pm));
	if (!skb)
		return -ENOMEM;

	pm = (struct wmi_power_params_cmd *)skb->data;
	pm->idle_period = cpu_to_le16(idle_period);
	pm->pspoll_number = cpu_to_le16(ps_poll_num);
	pm->dtim_policy = cpu_to_le16(dtim_policy);
	pm->tx_wakeup_policy = cpu_to_le16(tx_wakeup_policy);
	pm->num_tx_to_wakeup = cpu_to_le16(num_tx_to_wakeup);
	pm->ps_fail_event_policy = cpu_to_le16(ps_fail_event_policy);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_SET_POWER_PARAMS_CMDID,
				  NO_SYNC_WMIFLAG);
	return ret;
}