static void i40e_config_xps_tx_ring(struct i40e_ring *ring)
{
	int cpu;

	if (!ring->q_vector || !ring->netdev || ring->ch)
		return;

	/* We only initialize XPS once, so as not to overwrite user settings */
	if (test_and_set_bit(__I40E_TX_XPS_INIT_DONE, ring->state))
		return;

	cpu = cpumask_local_spread(ring->q_vector->v_idx, -1);
	netif_set_xps_queue(ring->netdev, get_cpu_mask(cpu),
			    ring->queue_index);
}