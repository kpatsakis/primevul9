void ath6kl_wmi_shutdown(struct wmi *wmi)
{
	if (!wmi)
		return;

	kfree(wmi->last_mgmt_tx_frame);
	kfree(wmi);
}