static struct nft_rule *__nf_tables_rule_lookup(const struct nft_chain *chain,
						u64 handle)
{
	struct nft_rule *rule;

	// FIXME: this sucks
	list_for_each_entry(rule, &chain->rules, list) {
		if (handle == rule->handle)
			return rule;
	}

	return ERR_PTR(-ENOENT);
}