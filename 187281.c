static void nfs4_check_drain_fc_complete(struct nfs4_session *ses)
{
	if (!test_bit(NFS4_SESSION_DRAINING, &ses->session_state)) {
		rpc_wake_up_first(&ses->fc_slot_table.slot_tbl_waitq,
				nfs4_set_task_privileged, NULL);
		return;
	}

	if (ses->fc_slot_table.highest_used_slotid != NFS4_NO_SLOT)
		return;

	dprintk("%s COMPLETE: Session Fore Channel Drained\n", __func__);
	complete(&ses->fc_slot_table.complete);
}