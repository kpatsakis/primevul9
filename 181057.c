static void ath6kl_wmi_hb_challenge_resp_event(struct wmi *wmi, u8 *datap,
					       int len)
{
	struct wmix_hb_challenge_resp_cmd *cmd;

	if (len < sizeof(struct wmix_hb_challenge_resp_cmd))
		return;

	cmd = (struct wmix_hb_challenge_resp_cmd *) datap;
	ath6kl_recovery_hb_event(wmi->parent_dev,
				 le32_to_cpu(cmd->cookie));
}