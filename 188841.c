static void i40e_netpoll(struct net_device *netdev)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	int i;

	/* if interface is down do nothing */
	if (test_bit(__I40E_VSI_DOWN, vsi->state))
		return;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		for (i = 0; i < vsi->num_q_vectors; i++)
			i40e_msix_clean_rings(0, vsi->q_vectors[i]);
	} else {
		i40e_intr(pf->pdev->irq, netdev);
	}
}