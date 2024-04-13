static int ll_temac_recv_buffers_available(struct temac_local *lp)
{
	int available;

	if (!lp->rx_skb[lp->rx_bd_ci])
		return 0;
	available = 1 + lp->rx_bd_tail - lp->rx_bd_ci;
	if (available <= 0)
		available += lp->rx_bd_num;
	return available;
}