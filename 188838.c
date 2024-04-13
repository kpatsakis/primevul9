static int i40e_configure_rx_ring(struct i40e_ring *ring)
{
	struct i40e_vsi *vsi = ring->vsi;
	u32 chain_len = vsi->back->hw.func_caps.rx_buf_chain_len;
	u16 pf_q = vsi->base_queue + ring->queue_index;
	struct i40e_hw *hw = &vsi->back->hw;
	struct i40e_hmc_obj_rxq rx_ctx;
	i40e_status err = 0;
	bool ok;
	int ret;

	bitmap_zero(ring->state, __I40E_RING_STATE_NBITS);

	/* clear the context structure first */
	memset(&rx_ctx, 0, sizeof(rx_ctx));

	if (ring->vsi->type == I40E_VSI_MAIN)
		xdp_rxq_info_unreg_mem_model(&ring->xdp_rxq);

	ring->xsk_umem = i40e_xsk_umem(ring);
	if (ring->xsk_umem) {
		ring->rx_buf_len = ring->xsk_umem->chunk_size_nohr -
				   XDP_PACKET_HEADROOM;
		/* For AF_XDP ZC, we disallow packets to span on
		 * multiple buffers, thus letting us skip that
		 * handling in the fast-path.
		 */
		chain_len = 1;
		ring->zca.free = i40e_zca_free;
		ret = xdp_rxq_info_reg_mem_model(&ring->xdp_rxq,
						 MEM_TYPE_ZERO_COPY,
						 &ring->zca);
		if (ret)
			return ret;
		dev_info(&vsi->back->pdev->dev,
			 "Registered XDP mem model MEM_TYPE_ZERO_COPY on Rx ring %d\n",
			 ring->queue_index);

	} else {
		ring->rx_buf_len = vsi->rx_buf_len;
		if (ring->vsi->type == I40E_VSI_MAIN) {
			ret = xdp_rxq_info_reg_mem_model(&ring->xdp_rxq,
							 MEM_TYPE_PAGE_SHARED,
							 NULL);
			if (ret)
				return ret;
		}
	}

	rx_ctx.dbuff = DIV_ROUND_UP(ring->rx_buf_len,
				    BIT_ULL(I40E_RXQ_CTX_DBUFF_SHIFT));

	rx_ctx.base = (ring->dma / 128);
	rx_ctx.qlen = ring->count;

	/* use 32 byte descriptors */
	rx_ctx.dsize = 1;

	/* descriptor type is always zero
	 * rx_ctx.dtype = 0;
	 */
	rx_ctx.hsplit_0 = 0;

	rx_ctx.rxmax = min_t(u16, vsi->max_frame, chain_len * ring->rx_buf_len);
	if (hw->revision_id == 0)
		rx_ctx.lrxqthresh = 0;
	else
		rx_ctx.lrxqthresh = 1;
	rx_ctx.crcstrip = 1;
	rx_ctx.l2tsel = 1;
	/* this controls whether VLAN is stripped from inner headers */
	rx_ctx.showiv = 0;
	/* set the prefena field to 1 because the manual says to */
	rx_ctx.prefena = 1;

	/* clear the context in the HMC */
	err = i40e_clear_lan_rx_queue_context(hw, pf_q);
	if (err) {
		dev_info(&vsi->back->pdev->dev,
			 "Failed to clear LAN Rx queue context on Rx ring %d (pf_q %d), error: %d\n",
			 ring->queue_index, pf_q, err);
		return -ENOMEM;
	}

	/* set the context in the HMC */
	err = i40e_set_lan_rx_queue_context(hw, pf_q, &rx_ctx);
	if (err) {
		dev_info(&vsi->back->pdev->dev,
			 "Failed to set LAN Rx queue context on Rx ring %d (pf_q %d), error: %d\n",
			 ring->queue_index, pf_q, err);
		return -ENOMEM;
	}

	/* configure Rx buffer alignment */
	if (!vsi->netdev || (vsi->back->flags & I40E_FLAG_LEGACY_RX))
		clear_ring_build_skb_enabled(ring);
	else
		set_ring_build_skb_enabled(ring);

	/* cache tail for quicker writes, and clear the reg before use */
	ring->tail = hw->hw_addr + I40E_QRX_TAIL(pf_q);
	writel(0, ring->tail);

	ok = ring->xsk_umem ?
	     i40e_alloc_rx_buffers_zc(ring, I40E_DESC_UNUSED(ring)) :
	     !i40e_alloc_rx_buffers(ring, I40E_DESC_UNUSED(ring));
	if (!ok) {
		/* Log this in case the user has forgotten to give the kernel
		 * any buffers, even later in the application.
		 */
		dev_info(&vsi->back->pdev->dev,
			 "Failed to allocate some buffers on %sRx ring %d (pf_q %d)\n",
			 ring->xsk_umem ? "UMEM enabled " : "",
			 ring->queue_index, pf_q);
	}

	return 0;
}