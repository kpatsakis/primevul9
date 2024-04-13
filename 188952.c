static void i40e_sync_udp_filters(struct i40e_pf *pf)
{
	int i;

	/* loop through and set pending bit for all active UDP filters */
	for (i = 0; i < I40E_MAX_PF_UDP_OFFLOAD_PORTS; i++) {
		if (pf->udp_ports[i].port)
			pf->pending_udp_bitmap |= BIT_ULL(i);
	}

	set_bit(__I40E_UDP_FILTER_SYNC_PENDING, pf->state);
}