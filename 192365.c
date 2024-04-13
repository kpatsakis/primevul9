static inline void skb_probe_transport_header(struct sk_buff *skb,
					      const int offset_hint)
{
	struct flow_keys keys;

	if (skb_transport_header_was_set(skb))
		return;
	else if (skb_flow_dissect(skb, &keys))
		skb_set_transport_header(skb, keys.thoff);
	else
		skb_set_transport_header(skb, offset_hint);
}