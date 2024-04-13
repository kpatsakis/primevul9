static void bnx2x_setup_dmae(struct bnx2x *bp)
{
	bp->dmae_ready = 0;
	spin_lock_init(&bp->dmae_lock);
}