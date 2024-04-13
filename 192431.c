static void *skb_flow_dissector_target(struct flow_dissector *flow_dissector,
				       enum flow_dissector_key_id key_id,
				       void *target_container)
{
	return ((char *) target_container) + flow_dissector->offset[key_id];
}