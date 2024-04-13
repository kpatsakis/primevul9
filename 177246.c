static inline struct sdma_txreq *get_txhead(struct sdma_engine *sde)
{
	return sde->tx_ring[sde->tx_head & sde->sdma_mask];
}