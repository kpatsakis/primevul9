void nft_data_uninit(const struct nft_data *data, enum nft_data_types type)
{
	switch (type) {
	case NFT_DATA_VALUE:
		return;
	case NFT_DATA_VERDICT:
		return nft_verdict_uninit(data);
	default:
		WARN_ON(1);
	}
}