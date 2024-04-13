static struct nft_rule *nf_tables_rule_lookup(const struct nft_chain *chain,
					      const struct nlattr *nla)
{
	if (nla == NULL)
		return ERR_PTR(-EINVAL);

	return __nf_tables_rule_lookup(chain, be64_to_cpu(nla_get_be64(nla)));
}