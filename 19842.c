static int copy_action(const struct nlattr *from,
		       struct sw_flow_actions **sfa, bool log)
{
	int totlen = NLA_ALIGN(from->nla_len);
	struct nlattr *to;

	to = reserve_sfa_size(sfa, from->nla_len, log);
	if (IS_ERR(to))
		return PTR_ERR(to);

	memcpy(to, from, totlen);
	return 0;
}