void ath6kl_wmi_set_control_ep(struct wmi *wmi, enum htc_endpoint_id ep_id)
{
	if (WARN_ON(ep_id == ENDPOINT_UNUSED || ep_id >= ENDPOINT_MAX))
		return;

	wmi->ep_id = ep_id;
}