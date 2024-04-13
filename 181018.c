static int ath6kl_wmi_host_sleep_mode_cmd_prcd_evt_rx(struct wmi *wmi,
						      struct ath6kl_vif *vif)
{
	struct ath6kl *ar = wmi->parent_dev;

	set_bit(HOST_SLEEP_MODE_CMD_PROCESSED, &vif->flags);
	wake_up(&ar->event_wq);

	return 0;
}