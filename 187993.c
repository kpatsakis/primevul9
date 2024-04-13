static void _prb_refresh_rx_retire_blk_timer(struct kbdq_core *pkc)
{
	mod_timer(&pkc->retire_blk_timer,
			jiffies + pkc->tov_in_jiffies);
	pkc->last_kactive_blk_num = pkc->kactive_blk_num;
}