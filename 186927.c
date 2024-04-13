void nfs4_check_drain_bc_complete(struct nfs4_session *ses)
{
	if (!test_bit(NFS4_SESSION_DRAINING, &ses->session_state) ||
	    ses->bc_slot_table.highest_used_slotid != NFS4_NO_SLOT)
		return;
	dprintk("%s COMPLETE: Session Back Channel Drained\n", __func__);
	complete(&ses->bc_slot_table.complete);
}