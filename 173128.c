static void bnx2x_init_cyclecounter(struct bnx2x *bp)
{
	memset(&bp->cyclecounter, 0, sizeof(bp->cyclecounter));
	bp->cyclecounter.read = bnx2x_cyclecounter_read;
	bp->cyclecounter.mask = CYCLECOUNTER_MASK(64);
	bp->cyclecounter.shift = 0;
	bp->cyclecounter.mult = 1;
}