int ovs_nla_get_identifier(struct sw_flow_id *sfid, const struct nlattr *ufid,
			   const struct sw_flow_key *key, bool log)
{
	struct sw_flow_key *new_key;

	if (ovs_nla_get_ufid(sfid, ufid, log))
		return 0;

	/* If UFID was not provided, use unmasked key. */
	new_key = kmalloc(sizeof(*new_key), GFP_KERNEL);
	if (!new_key)
		return -ENOMEM;
	memcpy(new_key, key, sizeof(*key));
	sfid->unmasked_key = new_key;

	return 0;
}