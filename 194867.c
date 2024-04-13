static void nft_chain_commit_update(struct nft_trans *trans)
{
	struct nft_base_chain *basechain;

	if (nft_trans_chain_name(trans)[0])
		strcpy(trans->ctx.chain->name, nft_trans_chain_name(trans));

	if (!(trans->ctx.chain->flags & NFT_BASE_CHAIN))
		return;

	basechain = nft_base_chain(trans->ctx.chain);
	nft_chain_stats_replace(basechain, nft_trans_chain_stats(trans));

	switch (nft_trans_chain_policy(trans)) {
	case NF_DROP:
	case NF_ACCEPT:
		basechain->policy = nft_trans_chain_policy(trans);
		break;
	}
}