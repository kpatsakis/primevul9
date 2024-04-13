static void bnx2x_setup_ndsb_state_machine(struct hc_status_block_sm *hc_sm,
					   int igu_sb_id, int igu_seg_id)
{
	hc_sm->igu_sb_id = igu_sb_id;
	hc_sm->igu_seg_id = igu_seg_id;
	hc_sm->timer_value = 0xFF;
	hc_sm->time_to_expire = 0xFFFFFFFF;
}