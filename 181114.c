int ath6kl_wmi_send_mgmt_cmd(struct wmi *wmi, u8 if_idx, u32 id, u32 freq,
				u32 wait, const u8 *data, u16 data_len,
				u32 no_cck)
{
	int status;
	struct ath6kl *ar = wmi->parent_dev;

	if (test_bit(ATH6KL_FW_CAPABILITY_STA_P2PDEV_DUPLEX,
		     ar->fw_capabilities)) {
		/*
		 * If capable of doing P2P mgmt operations using
		 * station interface, send additional information like
		 * supported rates to advertise and xmit rates for
		 * probe requests
		 */
		status = __ath6kl_wmi_send_mgmt_cmd(ar->wmi, if_idx, id, freq,
						    wait, data, data_len,
						    no_cck);
	} else {
		status = ath6kl_wmi_send_action_cmd(ar->wmi, if_idx, id, freq,
						    wait, data, data_len);
	}

	return status;
}