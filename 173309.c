static void bnx2x_release_firmware(struct bnx2x *bp)
{
	kfree(bp->init_ops_offsets);
	kfree(bp->init_ops);
	kfree(bp->init_data);
	release_firmware(bp->firmware);
	bp->firmware = NULL;
}