static void iwl_pcie_irq_set_affinity(struct iwl_trans *trans)
{
	int iter_rx_q, i, ret, cpu, offset;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	i = trans_pcie->shared_vec_mask & IWL_SHARED_IRQ_FIRST_RSS ? 0 : 1;
	iter_rx_q = trans_pcie->trans->num_rx_queues - 1 + i;
	offset = 1 + i;
	for (; i < iter_rx_q ; i++) {
		/*
		 * Get the cpu prior to the place to search
		 * (i.e. return will be > i - 1).
		 */
		cpu = cpumask_next(i - offset, cpu_online_mask);
		cpumask_set_cpu(cpu, &trans_pcie->affinity_mask[i]);
		ret = irq_set_affinity_hint(trans_pcie->msix_entries[i].vector,
					    &trans_pcie->affinity_mask[i]);
		if (ret)
			IWL_ERR(trans_pcie->trans,
				"Failed to set affinity mask for IRQ %d\n",
				i);
	}
}