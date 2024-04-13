static struct nlattr *__add_action(struct sw_flow_actions **sfa,
				   int attrtype, void *data, int len, bool log)
{
	struct nlattr *a;

	a = reserve_sfa_size(sfa, nla_attr_size(len), log);
	if (IS_ERR(a))
		return a;

	a->nla_type = attrtype;
	a->nla_len = nla_attr_size(len);

	if (data)
		memcpy(nla_data(a), data, len);
	memset((unsigned char *) a + a->nla_len, 0, nla_padlen(len));

	return a;
}