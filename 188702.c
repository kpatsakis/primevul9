static void i40e_vsi_clear_rings(struct i40e_vsi *vsi)
{
	int i;

	if (vsi->tx_rings && vsi->tx_rings[0]) {
		for (i = 0; i < vsi->alloc_queue_pairs; i++) {
			kfree_rcu(vsi->tx_rings[i], rcu);
			vsi->tx_rings[i] = NULL;
			vsi->rx_rings[i] = NULL;
			if (vsi->xdp_rings)
				vsi->xdp_rings[i] = NULL;
		}
	}
}