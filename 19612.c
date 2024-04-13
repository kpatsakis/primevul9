static u32 iwl_trans_pcie_dump_rbs(struct iwl_trans *trans,
				   struct iwl_fw_error_dump_data **data,
				   int allocated_rb_nums)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int max_len = PAGE_SIZE << trans_pcie->rx_page_order;
	/* Dump RBs is supported only for pre-9000 devices (1 queue) */
	struct iwl_rxq *rxq = &trans_pcie->rxq[0];
	u32 i, r, j, rb_len = 0;

	spin_lock(&rxq->lock);

	r = le16_to_cpu(iwl_get_closed_rb_stts(trans, rxq)) & 0x0FFF;

	for (i = rxq->read, j = 0;
	     i != r && j < allocated_rb_nums;
	     i = (i + 1) & RX_QUEUE_MASK, j++) {
		struct iwl_rx_mem_buffer *rxb = rxq->queue[i];
		struct iwl_fw_error_dump_rb *rb;

		dma_unmap_page(trans->dev, rxb->page_dma, max_len,
			       DMA_FROM_DEVICE);

		rb_len += sizeof(**data) + sizeof(*rb) + max_len;

		(*data)->type = cpu_to_le32(IWL_FW_ERROR_DUMP_RB);
		(*data)->len = cpu_to_le32(sizeof(*rb) + max_len);
		rb = (void *)(*data)->data;
		rb->index = cpu_to_le32(i);
		memcpy(rb->data, page_address(rxb->page), max_len);
		/* remap the page for the free benefit */
		rxb->page_dma = dma_map_page(trans->dev, rxb->page, 0,
						     max_len,
						     DMA_FROM_DEVICE);

		*data = iwl_fw_error_next_data(*data);
	}

	spin_unlock(&rxq->lock);

	return rb_len;
}