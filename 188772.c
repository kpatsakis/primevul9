static int i40e_vsi_configure_rx(struct i40e_vsi *vsi)
{
	int err = 0;
	u16 i;

	if (!vsi->netdev || (vsi->back->flags & I40E_FLAG_LEGACY_RX)) {
		vsi->max_frame = I40E_MAX_RXBUFFER;
		vsi->rx_buf_len = I40E_RXBUFFER_2048;
#if (PAGE_SIZE < 8192)
	} else if (!I40E_2K_TOO_SMALL_WITH_PADDING &&
		   (vsi->netdev->mtu <= ETH_DATA_LEN)) {
		vsi->max_frame = I40E_RXBUFFER_1536 - NET_IP_ALIGN;
		vsi->rx_buf_len = I40E_RXBUFFER_1536 - NET_IP_ALIGN;
#endif
	} else {
		vsi->max_frame = I40E_MAX_RXBUFFER;
		vsi->rx_buf_len = (PAGE_SIZE < 8192) ? I40E_RXBUFFER_3072 :
						       I40E_RXBUFFER_2048;
	}

	/* set up individual rings */
	for (i = 0; i < vsi->num_queue_pairs && !err; i++)
		err = i40e_configure_rx_ring(vsi->rx_rings[i]);

	return err;
}