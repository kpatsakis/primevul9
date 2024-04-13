static __inline__ void isdn_net_dec_frame_cnt(isdn_net_local *lp)
{
	atomic_dec(&lp->frame_cnt);

	if (!(isdn_net_device_busy(lp))) {
		if (!skb_queue_empty(&lp->super_tx_queue)) {
			schedule_work(&lp->tqueue);
		} else {
			isdn_net_device_wake_queue(lp);
		}
	}
}