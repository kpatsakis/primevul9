int nft_register_chain_type(const struct nf_chain_type *ctype)
{
	int err = 0;

	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	if (chain_type[ctype->family][ctype->type] != NULL) {
		err = -EBUSY;
		goto out;
	}
	chain_type[ctype->family][ctype->type] = ctype;
out:
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
	return err;
}