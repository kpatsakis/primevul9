static int i40e_xdp(struct net_device *dev,
		    struct netdev_bpf *xdp)
{
	struct i40e_netdev_priv *np = netdev_priv(dev);
	struct i40e_vsi *vsi = np->vsi;

	if (vsi->type != I40E_VSI_MAIN)
		return -EINVAL;

	switch (xdp->command) {
	case XDP_SETUP_PROG:
		return i40e_xdp_setup(vsi, xdp->prog);
	case XDP_QUERY_PROG:
		xdp->prog_id = vsi->xdp_prog ? vsi->xdp_prog->aux->id : 0;
		return 0;
	case XDP_SETUP_XSK_UMEM:
		return i40e_xsk_umem_setup(vsi, xdp->xsk.umem,
					   xdp->xsk.queue_id);
	default:
		return -EINVAL;
	}
}