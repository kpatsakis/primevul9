static int ath6kl_wmi_ready_event_rx(struct wmi *wmi, u8 *datap, int len)
{
	struct wmi_ready_event_2 *ev = (struct wmi_ready_event_2 *) datap;

	if (len < sizeof(struct wmi_ready_event_2))
		return -EINVAL;

	ath6kl_ready_event(wmi->parent_dev, ev->mac_addr,
			   le32_to_cpu(ev->sw_version),
			   le32_to_cpu(ev->abi_version), ev->phy_cap);

	return 0;
}