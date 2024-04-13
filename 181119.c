static int ath6kl_wmi_meta_add(struct wmi *wmi, struct sk_buff *skb,
			       u8 *version, void *tx_meta_info)
{
	struct wmi_tx_meta_v1 *v1;
	struct wmi_tx_meta_v2 *v2;

	if (WARN_ON(skb == NULL || version == NULL))
		return -EINVAL;

	switch (*version) {
	case WMI_META_VERSION_1:
		skb_push(skb, WMI_MAX_TX_META_SZ);
		v1 = (struct wmi_tx_meta_v1 *) skb->data;
		v1->pkt_id = 0;
		v1->rate_plcy_id = 0;
		*version = WMI_META_VERSION_1;
		break;
	case WMI_META_VERSION_2:
		skb_push(skb, WMI_MAX_TX_META_SZ);
		v2 = (struct wmi_tx_meta_v2 *) skb->data;
		memcpy(v2, (struct wmi_tx_meta_v2 *) tx_meta_info,
		       sizeof(struct wmi_tx_meta_v2));
		break;
	}

	return 0;
}