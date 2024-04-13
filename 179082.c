int peak_usb_netif_rx(struct sk_buff *skb,
		      struct peak_time_ref *time_ref, u32 ts_low)
{
	struct skb_shared_hwtstamps *hwts = skb_hwtstamps(skb);

	peak_usb_get_ts_time(time_ref, ts_low, &hwts->hwtstamp);

	return netif_rx(skb);
}