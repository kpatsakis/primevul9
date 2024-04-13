void skb_flow_dissector_init(struct flow_dissector *flow_dissector,
			     const struct flow_dissector_key *key,
			     unsigned int key_count)
{
	unsigned int i;

	memset(flow_dissector, 0, sizeof(*flow_dissector));

	for (i = 0; i < key_count; i++, key++) {
		/* User should make sure that every key target offset is withing
		 * boundaries of unsigned short.
		 */
		BUG_ON(key->offset > USHRT_MAX);
		BUG_ON(skb_flow_dissector_uses_key(flow_dissector,
						   key->key_id));

		skb_flow_dissector_set_key(flow_dissector, key->key_id);
		flow_dissector->offset[key->key_id] = key->offset;
	}

	/* Ensure that the dissector always includes control and basic key.
	 * That way we are able to avoid handling lack of these in fast path.
	 */
	BUG_ON(!skb_flow_dissector_uses_key(flow_dissector,
					    FLOW_DISSECTOR_KEY_CONTROL));
	BUG_ON(!skb_flow_dissector_uses_key(flow_dissector,
					    FLOW_DISSECTOR_KEY_BASIC));
}