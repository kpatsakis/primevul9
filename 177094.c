static int gfs2_rbm_from_block(struct gfs2_rbm *rbm, u64 block)
{
	u64 rblock = block - rbm->rgd->rd_data0;

	if (WARN_ON_ONCE(rblock > UINT_MAX))
		return -EINVAL;
	if (block >= rbm->rgd->rd_data0 + rbm->rgd->rd_data)
		return -E2BIG;

	rbm->bii = 0;
	rbm->offset = (u32)(rblock);
	/* Check if the block is within the first block */
	if (rbm->offset < rbm_bi(rbm)->bi_blocks)
		return 0;

	/* Adjust for the size diff between gfs2_meta_header and gfs2_rgrp */
	rbm->offset += (sizeof(struct gfs2_rgrp) -
			sizeof(struct gfs2_meta_header)) * GFS2_NBBY;
	rbm->bii = rbm->offset / rbm->rgd->rd_sbd->sd_blocks_per_bitmap;
	rbm->offset -= rbm->bii * rbm->rgd->rd_sbd->sd_blocks_per_bitmap;
	return 0;
}