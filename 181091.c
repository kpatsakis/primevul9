int ath6kl_wmi_control_rx(struct wmi *wmi, struct sk_buff *skb)
{
	if (WARN_ON(skb == NULL))
		return -EINVAL;

	if (skb->len < sizeof(struct wmi_cmd_hdr)) {
		ath6kl_err("bad packet 1\n");
		dev_kfree_skb(skb);
		return -EINVAL;
	}

	trace_ath6kl_wmi_event(skb->data, skb->len);

	return ath6kl_wmi_proc_events(wmi, skb);
}