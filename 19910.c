static bool validate_nsh(const struct nlattr *attr, bool is_mask,
			 bool is_push_nsh, bool log)
{
	struct sw_flow_match match;
	struct sw_flow_key key;
	int ret = 0;

	ovs_match_init(&match, &key, true, NULL);
	ret = nsh_key_put_from_nlattr(attr, &match, is_mask,
				      is_push_nsh, log);
	return !ret;
}