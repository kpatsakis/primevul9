static __inline__ void isdn_net_zero_frame_cnt(isdn_net_local *lp)
{
	atomic_set(&lp->frame_cnt, 0);
}