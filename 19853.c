void ovs_match_init(struct sw_flow_match *match,
		    struct sw_flow_key *key,
		    bool reset_key,
		    struct sw_flow_mask *mask)
{
	memset(match, 0, sizeof(*match));
	match->key = key;
	match->mask = mask;

	if (reset_key)
		memset(key, 0, sizeof(*key));

	if (mask) {
		memset(&mask->key, 0, sizeof(mask->key));
		mask->range.start = mask->range.end = 0;
	}
}