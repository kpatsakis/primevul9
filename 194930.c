static struct nft_stats __percpu *nft_stats_alloc(const struct nlattr *attr)
{
	struct nlattr *tb[NFTA_COUNTER_MAX+1];
	struct nft_stats __percpu *newstats;
	struct nft_stats *stats;
	int err;

	err = nla_parse_nested(tb, NFTA_COUNTER_MAX, attr, nft_counter_policy);
	if (err < 0)
		return ERR_PTR(err);

	if (!tb[NFTA_COUNTER_BYTES] || !tb[NFTA_COUNTER_PACKETS])
		return ERR_PTR(-EINVAL);

	newstats = netdev_alloc_pcpu_stats(struct nft_stats);
	if (newstats == NULL)
		return ERR_PTR(-ENOMEM);

	/* Restore old counters on this cpu, no problem. Per-cpu statistics
	 * are not exposed to userspace.
	 */
	stats = this_cpu_ptr(newstats);
	stats->bytes = be64_to_cpu(nla_get_be64(tb[NFTA_COUNTER_BYTES]));
	stats->pkts = be64_to_cpu(nla_get_be64(tb[NFTA_COUNTER_PACKETS]));

	return newstats;
}