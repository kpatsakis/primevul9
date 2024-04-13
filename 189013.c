static u8 i40e_get_udp_port_idx(struct i40e_pf *pf, u16 port)
{
	u8 i;

	for (i = 0; i < I40E_MAX_PF_UDP_OFFLOAD_PORTS; i++) {
		/* Do not report ports with pending deletions as
		 * being available.
		 */
		if (!port && (pf->pending_udp_bitmap & BIT_ULL(i)))
			continue;
		if (pf->udp_ports[i].port == port)
			return i;
	}

	return i;
}