static int ath6kl_wmi_get_pmkid_list_event_rx(struct wmi *wmi, u8 *datap,
					      u32 len)
{
	struct wmi_pmkid_list_reply *reply;
	u32 expected_len;

	if (len < sizeof(struct wmi_pmkid_list_reply))
		return -EINVAL;

	reply = (struct wmi_pmkid_list_reply *)datap;
	expected_len = sizeof(reply->num_pmkid) +
		le32_to_cpu(reply->num_pmkid) * WMI_PMKID_LEN;

	if (len < expected_len)
		return -EINVAL;

	return 0;
}