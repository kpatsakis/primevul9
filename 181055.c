static int ath6kl_wmi_data_sync_send(struct wmi *wmi, struct sk_buff *skb,
			      enum htc_endpoint_id ep_id, u8 if_idx)
{
	struct wmi_data_hdr *data_hdr;
	int ret;

	if (WARN_ON(skb == NULL || ep_id == wmi->ep_id)) {
		dev_kfree_skb(skb);
		return -EINVAL;
	}

	skb_push(skb, sizeof(struct wmi_data_hdr));

	data_hdr = (struct wmi_data_hdr *) skb->data;
	data_hdr->info = SYNC_MSGTYPE << WMI_DATA_HDR_MSG_TYPE_SHIFT;
	data_hdr->info3 = cpu_to_le16(if_idx & WMI_DATA_HDR_IF_IDX_MASK);

	ret = ath6kl_control_tx(wmi->parent_dev, skb, ep_id);

	return ret;
}