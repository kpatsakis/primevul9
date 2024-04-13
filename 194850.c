static inline int gencursor_next(struct net *net)
{
	return net->nft.gencursor+1 == 1 ? 1 : 0;
}