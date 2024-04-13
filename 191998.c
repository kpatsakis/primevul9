static u32 rtl8xxxu_queue_select(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	u32 queue;

	if (ieee80211_is_mgmt(hdr->frame_control))
		queue = TXDESC_QUEUE_MGNT;
	else
		queue = rtl8xxxu_80211_to_rtl_queue(skb_get_queue_mapping(skb));

	return queue;
}