static void nfs4_free_closedata(void *data)
{
	struct nfs4_closedata *calldata = data;
	struct nfs4_state_owner *sp = calldata->state->owner;
	struct super_block *sb = calldata->state->inode->i_sb;

	if (calldata->roc)
		pnfs_roc_release(calldata->state->inode);
	nfs4_put_open_state(calldata->state);
	nfs_free_seqid(calldata->arg.seqid);
	nfs4_put_state_owner(sp);
	nfs_sb_deactive(sb);
	kfree(calldata);
}