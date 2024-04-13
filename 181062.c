void ath6kl_wmi_reset(struct wmi *wmi)
{
	spin_lock_bh(&wmi->lock);

	wmi->fat_pipe_exist = 0;
	memset(wmi->stream_exist_for_ac, 0, sizeof(wmi->stream_exist_for_ac));

	spin_unlock_bh(&wmi->lock);
}