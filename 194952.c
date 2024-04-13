int nft_chain_validate_dependency(const struct nft_chain *chain,
				  enum nft_chain_type type)
{
	const struct nft_base_chain *basechain;

	if (chain->flags & NFT_BASE_CHAIN) {
		basechain = nft_base_chain(chain);
		if (basechain->type->type != type)
			return -EOPNOTSUPP;
	}
	return 0;
}