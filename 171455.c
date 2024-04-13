
static void ql_free_large_buffers(struct ql3_adapter *qdev)
{
	int i = 0;
	struct ql_rcv_buf_cb *lrg_buf_cb;

	for (i = 0; i < qdev->num_large_buffers; i++) {
		lrg_buf_cb = &qdev->lrg_buf[i];
		if (lrg_buf_cb->skb) {
			dev_kfree_skb(lrg_buf_cb->skb);
			pci_unmap_single(qdev->pdev,
					 dma_unmap_addr(lrg_buf_cb, mapaddr),
					 dma_unmap_len(lrg_buf_cb, maplen),
					 PCI_DMA_FROMDEVICE);
			memset(lrg_buf_cb, 0, sizeof(struct ql_rcv_buf_cb));
		} else {
			break;
		}
	}