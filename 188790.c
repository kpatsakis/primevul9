static struct xdp_umem *i40e_xsk_umem(struct i40e_ring *ring)
{
	bool xdp_on = i40e_enabled_xdp_vsi(ring->vsi);
	int qid = ring->queue_index;

	if (ring_is_xdp(ring))
		qid -= ring->vsi->alloc_queue_pairs;

	if (!xdp_on || !test_bit(qid, ring->vsi->af_xdp_zc_qps))
		return NULL;

	return xdp_get_umem_from_qid(ring->vsi->netdev, qid);
}