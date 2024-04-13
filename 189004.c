static int i40e_xdp_setup(struct i40e_vsi *vsi,
			  struct bpf_prog *prog)
{
	int frame_size = vsi->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;
	struct i40e_pf *pf = vsi->back;
	struct bpf_prog *old_prog;
	bool need_reset;
	int i;

	/* Don't allow frames that span over multiple buffers */
	if (frame_size > vsi->rx_buf_len)
		return -EINVAL;

	if (!i40e_enabled_xdp_vsi(vsi) && !prog)
		return 0;

	/* When turning XDP on->off/off->on we reset and rebuild the rings. */
	need_reset = (i40e_enabled_xdp_vsi(vsi) != !!prog);

	if (need_reset)
		i40e_prep_for_reset(pf, true);

	old_prog = xchg(&vsi->xdp_prog, prog);

	if (need_reset)
		i40e_reset_and_rebuild(pf, true, true);

	for (i = 0; i < vsi->num_queue_pairs; i++)
		WRITE_ONCE(vsi->rx_rings[i]->xdp_prog, vsi->xdp_prog);

	if (old_prog)
		bpf_prog_put(old_prog);

	/* Kick start the NAPI context if there is an AF_XDP socket open
	 * on that queue id. This so that receiving will start.
	 */
	if (need_reset && prog)
		for (i = 0; i < vsi->num_queue_pairs; i++)
			if (vsi->xdp_rings[i]->xsk_umem)
				(void)i40e_xsk_wakeup(vsi->netdev, i,
						      XDP_WAKEUP_RX);

	return 0;
}