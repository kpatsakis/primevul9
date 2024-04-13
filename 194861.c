static struct nft_af_info *nft_afinfo_lookup(struct net *net, int family)
{
	struct nft_af_info *afi;

	list_for_each_entry(afi, &net->nft.af_info, list) {
		if (afi->family == family)
			return afi;
	}
	return NULL;
}