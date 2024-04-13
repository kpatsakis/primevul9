static int i40e_configure_tx_ring(struct i40e_ring *ring)
{
	struct i40e_vsi *vsi = ring->vsi;
	u16 pf_q = vsi->base_queue + ring->queue_index;
	struct i40e_hw *hw = &vsi->back->hw;
	struct i40e_hmc_obj_txq tx_ctx;
	i40e_status err = 0;
	u32 qtx_ctl = 0;

	if (ring_is_xdp(ring))
		ring->xsk_umem = i40e_xsk_umem(ring);

	/* some ATR related tx ring init */
	if (vsi->back->flags & I40E_FLAG_FD_ATR_ENABLED) {
		ring->atr_sample_rate = vsi->back->atr_sample_rate;
		ring->atr_count = 0;
	} else {
		ring->atr_sample_rate = 0;
	}

	/* configure XPS */
	i40e_config_xps_tx_ring(ring);

	/* clear the context structure first */
	memset(&tx_ctx, 0, sizeof(tx_ctx));

	tx_ctx.new_context = 1;
	tx_ctx.base = (ring->dma / 128);
	tx_ctx.qlen = ring->count;
	tx_ctx.fd_ena = !!(vsi->back->flags & (I40E_FLAG_FD_SB_ENABLED |
					       I40E_FLAG_FD_ATR_ENABLED));
	tx_ctx.timesync_ena = !!(vsi->back->flags & I40E_FLAG_PTP);
	/* FDIR VSI tx ring can still use RS bit and writebacks */
	if (vsi->type != I40E_VSI_FDIR)
		tx_ctx.head_wb_ena = 1;
	tx_ctx.head_wb_addr = ring->dma +
			      (ring->count * sizeof(struct i40e_tx_desc));

	/* As part of VSI creation/update, FW allocates certain
	 * Tx arbitration queue sets for each TC enabled for
	 * the VSI. The FW returns the handles to these queue
	 * sets as part of the response buffer to Add VSI,
	 * Update VSI, etc. AQ commands. It is expected that
	 * these queue set handles be associated with the Tx
	 * queues by the driver as part of the TX queue context
	 * initialization. This has to be done regardless of
	 * DCB as by default everything is mapped to TC0.
	 */

	if (ring->ch)
		tx_ctx.rdylist =
			le16_to_cpu(ring->ch->info.qs_handle[ring->dcb_tc]);

	else
		tx_ctx.rdylist = le16_to_cpu(vsi->info.qs_handle[ring->dcb_tc]);

	tx_ctx.rdylist_act = 0;

	/* clear the context in the HMC */
	err = i40e_clear_lan_tx_queue_context(hw, pf_q);
	if (err) {
		dev_info(&vsi->back->pdev->dev,
			 "Failed to clear LAN Tx queue context on Tx ring %d (pf_q %d), error: %d\n",
			 ring->queue_index, pf_q, err);
		return -ENOMEM;
	}

	/* set the context in the HMC */
	err = i40e_set_lan_tx_queue_context(hw, pf_q, &tx_ctx);
	if (err) {
		dev_info(&vsi->back->pdev->dev,
			 "Failed to set LAN Tx queue context on Tx ring %d (pf_q %d, error: %d\n",
			 ring->queue_index, pf_q, err);
		return -ENOMEM;
	}

	/* Now associate this queue with this PCI function */
	if (ring->ch) {
		if (ring->ch->type == I40E_VSI_VMDQ2)
			qtx_ctl = I40E_QTX_CTL_VM_QUEUE;
		else
			return -EINVAL;

		qtx_ctl |= (ring->ch->vsi_number <<
			    I40E_QTX_CTL_VFVM_INDX_SHIFT) &
			    I40E_QTX_CTL_VFVM_INDX_MASK;
	} else {
		if (vsi->type == I40E_VSI_VMDQ2) {
			qtx_ctl = I40E_QTX_CTL_VM_QUEUE;
			qtx_ctl |= ((vsi->id) << I40E_QTX_CTL_VFVM_INDX_SHIFT) &
				    I40E_QTX_CTL_VFVM_INDX_MASK;
		} else {
			qtx_ctl = I40E_QTX_CTL_PF_QUEUE;
		}
	}

	qtx_ctl |= ((hw->pf_id << I40E_QTX_CTL_PF_INDX_SHIFT) &
		    I40E_QTX_CTL_PF_INDX_MASK);
	wr32(hw, I40E_QTX_CTL(pf_q), qtx_ctl);
	i40e_flush(hw);

	/* cache tail off for easier writes later */
	ring->tail = hw->hw_addr + I40E_QTX_TAIL(pf_q);

	return 0;
}