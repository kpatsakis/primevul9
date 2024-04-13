static void nft_verdict_uninit(const struct nft_data *data)
{
	switch (data->verdict) {
	case NFT_JUMP:
	case NFT_GOTO:
		data->chain->use--;
		break;
	}
}