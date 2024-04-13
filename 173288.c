static int bnx2x_gunzip_init(struct bnx2x *bp)
{
	bp->gunzip_buf = dma_alloc_coherent(&bp->pdev->dev, FW_BUF_SIZE,
					    &bp->gunzip_mapping, GFP_KERNEL);
	if (bp->gunzip_buf  == NULL)
		goto gunzip_nomem1;

	bp->strm = kmalloc(sizeof(*bp->strm), GFP_KERNEL);
	if (bp->strm  == NULL)
		goto gunzip_nomem2;

	bp->strm->workspace = vmalloc(zlib_inflate_workspacesize());
	if (bp->strm->workspace == NULL)
		goto gunzip_nomem3;

	return 0;

gunzip_nomem3:
	kfree(bp->strm);
	bp->strm = NULL;

gunzip_nomem2:
	dma_free_coherent(&bp->pdev->dev, FW_BUF_SIZE, bp->gunzip_buf,
			  bp->gunzip_mapping);
	bp->gunzip_buf = NULL;

gunzip_nomem1:
	BNX2X_ERR("Cannot allocate firmware buffer for un-compression\n");
	return -ENOMEM;
}