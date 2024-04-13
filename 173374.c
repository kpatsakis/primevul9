static void poll_bnx2x(struct net_device *dev)
{
	struct bnx2x *bp = netdev_priv(dev);
	int i;

	for_each_eth_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];
		napi_schedule(&bnx2x_fp(bp, fp->index, napi));
	}
}