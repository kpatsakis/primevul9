static inline int cgw_register_filter(struct net *net, struct cgw_job *gwj)
{
	return can_rx_register(net, gwj->src.dev, gwj->ccgw.filter.can_id,
			       gwj->ccgw.filter.can_mask, can_can_gw_rcv,
			       gwj, "gw", NULL);
}