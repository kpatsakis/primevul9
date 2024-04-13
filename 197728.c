static inline void cgw_unregister_filter(struct net *net, struct cgw_job *gwj)
{
	can_rx_unregister(net, gwj->src.dev, gwj->ccgw.filter.can_id,
			  gwj->ccgw.filter.can_mask, can_can_gw_rcv, gwj);
}