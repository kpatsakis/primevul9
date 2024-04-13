static int i40e_alloc_rings(struct i40e_vsi *vsi)
{
	int i, qpv = i40e_enabled_xdp_vsi(vsi) ? 3 : 2;
	struct i40e_pf *pf = vsi->back;
	struct i40e_ring *ring;

	/* Set basic values in the rings to be used later during open() */
	for (i = 0; i < vsi->alloc_queue_pairs; i++) {
		/* allocate space for both Tx and Rx in one shot */
		ring = kcalloc(qpv, sizeof(struct i40e_ring), GFP_KERNEL);
		if (!ring)
			goto err_out;

		ring->queue_index = i;
		ring->reg_idx = vsi->base_queue + i;
		ring->ring_active = false;
		ring->vsi = vsi;
		ring->netdev = vsi->netdev;
		ring->dev = &pf->pdev->dev;
		ring->count = vsi->num_tx_desc;
		ring->size = 0;
		ring->dcb_tc = 0;
		if (vsi->back->hw_features & I40E_HW_WB_ON_ITR_CAPABLE)
			ring->flags = I40E_TXR_FLAGS_WB_ON_ITR;
		ring->itr_setting = pf->tx_itr_default;
		vsi->tx_rings[i] = ring++;

		if (!i40e_enabled_xdp_vsi(vsi))
			goto setup_rx;

		ring->queue_index = vsi->alloc_queue_pairs + i;
		ring->reg_idx = vsi->base_queue + ring->queue_index;
		ring->ring_active = false;
		ring->vsi = vsi;
		ring->netdev = NULL;
		ring->dev = &pf->pdev->dev;
		ring->count = vsi->num_tx_desc;
		ring->size = 0;
		ring->dcb_tc = 0;
		if (vsi->back->hw_features & I40E_HW_WB_ON_ITR_CAPABLE)
			ring->flags = I40E_TXR_FLAGS_WB_ON_ITR;
		set_ring_xdp(ring);
		ring->itr_setting = pf->tx_itr_default;
		vsi->xdp_rings[i] = ring++;

setup_rx:
		ring->queue_index = i;
		ring->reg_idx = vsi->base_queue + i;
		ring->ring_active = false;
		ring->vsi = vsi;
		ring->netdev = vsi->netdev;
		ring->dev = &pf->pdev->dev;
		ring->count = vsi->num_rx_desc;
		ring->size = 0;
		ring->dcb_tc = 0;
		ring->itr_setting = pf->rx_itr_default;
		vsi->rx_rings[i] = ring;
	}

	return 0;

err_out:
	i40e_vsi_clear_rings(vsi);
	return -ENOMEM;
}