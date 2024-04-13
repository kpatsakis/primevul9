void iwl_trans_pcie_free(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int i;

	iwl_pcie_synchronize_irqs(trans);

	if (trans->trans_cfg->gen2)
		iwl_pcie_gen2_tx_free(trans);
	else
		iwl_pcie_tx_free(trans);
	iwl_pcie_rx_free(trans);

	if (trans_pcie->rba.alloc_wq) {
		destroy_workqueue(trans_pcie->rba.alloc_wq);
		trans_pcie->rba.alloc_wq = NULL;
	}

	if (trans_pcie->msix_enabled) {
		for (i = 0; i < trans_pcie->alloc_vecs; i++) {
			irq_set_affinity_hint(
				trans_pcie->msix_entries[i].vector,
				NULL);
		}

		trans_pcie->msix_enabled = false;
	} else {
		iwl_pcie_free_ict(trans);
	}

	iwl_pcie_free_fw_monitor(trans);

	for_each_possible_cpu(i) {
		struct iwl_tso_hdr_page *p =
			per_cpu_ptr(trans_pcie->tso_hdr_page, i);

		if (p->page)
			__free_page(p->page);
	}

	free_percpu(trans_pcie->tso_hdr_page);
	mutex_destroy(&trans_pcie->mutex);
	iwl_trans_free(trans);
}