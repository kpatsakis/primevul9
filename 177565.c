static __inline__ void isdn_net_inc_frame_cnt(isdn_net_local *lp)
{
	atomic_inc(&lp->frame_cnt);
	if (isdn_net_device_busy(lp))
		isdn_net_device_stop_queue(lp);
}