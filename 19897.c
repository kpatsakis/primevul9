int ovs_nla_add_action(struct sw_flow_actions **sfa, int attrtype, void *data,
		       int len, bool log)
{
	struct nlattr *a;

	a = __add_action(sfa, attrtype, data, len, log);

	return PTR_ERR_OR_ZERO(a);
}