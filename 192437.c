static bool skb_flow_dissector_uses_key(struct flow_dissector *flow_dissector,
					enum flow_dissector_key_id key_id)
{
	return flow_dissector->used_keys & (1 << key_id);
}