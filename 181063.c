void *ath6kl_wmi_init(struct ath6kl *dev)
{
	struct wmi *wmi;

	wmi = kzalloc(sizeof(struct wmi), GFP_KERNEL);
	if (!wmi)
		return NULL;

	spin_lock_init(&wmi->lock);

	wmi->parent_dev = dev;

	wmi->pwr_mode = REC_POWER;

	ath6kl_wmi_reset(wmi);

	return wmi;
}