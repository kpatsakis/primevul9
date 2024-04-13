int nft_validate_data_load(const struct nft_ctx *ctx, enum nft_registers reg,
			   const struct nft_data *data,
			   enum nft_data_types type)
{
	int err;

	switch (reg) {
	case NFT_REG_VERDICT:
		if (data == NULL || type != NFT_DATA_VERDICT)
			return -EINVAL;

		if (data->verdict == NFT_GOTO || data->verdict == NFT_JUMP) {
			err = nf_tables_check_loops(ctx, data->chain);
			if (err < 0)
				return err;

			if (ctx->chain->level + 1 > data->chain->level) {
				if (ctx->chain->level + 1 == NFT_JUMP_STACK_SIZE)
					return -EMLINK;
				data->chain->level = ctx->chain->level + 1;
			}
		}

		return 0;
	default:
		if (data != NULL && type != NFT_DATA_VALUE)
			return -EINVAL;
		return 0;
	}
}