int bnx2x_setup_leading(struct bnx2x *bp)
{
	if (IS_PF(bp))
		return bnx2x_setup_queue(bp, &bp->fp[0], true);
	else /* VF */
		return bnx2x_vfpf_setup_q(bp, &bp->fp[0], true);
}