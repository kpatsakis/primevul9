static void bnx2x_gunzip_end(struct bnx2x *bp)
{
	if (bp->strm) {
		vfree(bp->strm->workspace);
		kfree(bp->strm);
		bp->strm = NULL;
	}

	if (bp->gunzip_buf) {
		dma_free_coherent(&bp->pdev->dev, FW_BUF_SIZE, bp->gunzip_buf,
				  bp->gunzip_mapping);
		bp->gunzip_buf = NULL;
	}
}