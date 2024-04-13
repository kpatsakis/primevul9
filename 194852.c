static void nft_chain_stats_replace(struct nft_base_chain *chain,
				    struct nft_stats __percpu *newstats)
{
	if (newstats == NULL)
		return;

	if (chain->stats) {
		struct nft_stats __percpu *oldstats =
				nft_dereference(chain->stats);

		rcu_assign_pointer(chain->stats, newstats);
		synchronize_rcu();
		free_percpu(oldstats);
	} else
		rcu_assign_pointer(chain->stats, newstats);
}