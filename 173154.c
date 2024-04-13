static int bnx2x_cnic_sp_queue(struct net_device *dev,
			       struct kwqe_16 *kwqes[], u32 count)
{
	struct bnx2x *bp = netdev_priv(dev);
	int i;

#ifdef BNX2X_STOP_ON_ERROR
	if (unlikely(bp->panic)) {
		BNX2X_ERR("Can't post to SP queue while panic\n");
		return -EIO;
	}
#endif

	if ((bp->recovery_state != BNX2X_RECOVERY_DONE) &&
	    (bp->recovery_state != BNX2X_RECOVERY_NIC_LOADING)) {
		BNX2X_ERR("Handling parity error recovery. Try again later\n");
		return -EAGAIN;
	}

	spin_lock_bh(&bp->spq_lock);

	for (i = 0; i < count; i++) {
		struct eth_spe *spe = (struct eth_spe *)kwqes[i];

		if (bp->cnic_kwq_pending == MAX_SP_DESC_CNT)
			break;

		*bp->cnic_kwq_prod = *spe;

		bp->cnic_kwq_pending++;

		DP(BNX2X_MSG_SP, "L5 SPQE %x %x %x:%x pos %d\n",
		   spe->hdr.conn_and_cmd_data, spe->hdr.type,
		   spe->data.update_data_addr.hi,
		   spe->data.update_data_addr.lo,
		   bp->cnic_kwq_pending);

		if (bp->cnic_kwq_prod == bp->cnic_kwq_last)
			bp->cnic_kwq_prod = bp->cnic_kwq;
		else
			bp->cnic_kwq_prod++;
	}

	spin_unlock_bh(&bp->spq_lock);

	if (bp->cnic_spq_pending < bp->cnic_eth_dev.max_kwqe_pending)
		bnx2x_cnic_sp_post(bp, 0);

	return i;
}