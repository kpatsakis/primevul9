iwl_pcie_set_interrupt_capa(struct pci_dev *pdev,
			    struct iwl_trans *trans,
			    const struct iwl_cfg_trans_params *cfg_trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int max_irqs, num_irqs, i, ret;
	u16 pci_cmd;

	if (!cfg_trans->mq_rx_supported)
		goto enable_msi;

	max_irqs = min_t(u32, num_online_cpus() + 2, IWL_MAX_RX_HW_QUEUES);
	for (i = 0; i < max_irqs; i++)
		trans_pcie->msix_entries[i].entry = i;

	num_irqs = pci_enable_msix_range(pdev, trans_pcie->msix_entries,
					 MSIX_MIN_INTERRUPT_VECTORS,
					 max_irqs);
	if (num_irqs < 0) {
		IWL_DEBUG_INFO(trans,
			       "Failed to enable msi-x mode (ret %d). Moving to msi mode.\n",
			       num_irqs);
		goto enable_msi;
	}
	trans_pcie->def_irq = (num_irqs == max_irqs) ? num_irqs - 1 : 0;

	IWL_DEBUG_INFO(trans,
		       "MSI-X enabled. %d interrupt vectors were allocated\n",
		       num_irqs);

	/*
	 * In case the OS provides fewer interrupts than requested, different
	 * causes will share the same interrupt vector as follows:
	 * One interrupt less: non rx causes shared with FBQ.
	 * Two interrupts less: non rx causes shared with FBQ and RSS.
	 * More than two interrupts: we will use fewer RSS queues.
	 */
	if (num_irqs <= max_irqs - 2) {
		trans_pcie->trans->num_rx_queues = num_irqs + 1;
		trans_pcie->shared_vec_mask = IWL_SHARED_IRQ_NON_RX |
			IWL_SHARED_IRQ_FIRST_RSS;
	} else if (num_irqs == max_irqs - 1) {
		trans_pcie->trans->num_rx_queues = num_irqs;
		trans_pcie->shared_vec_mask = IWL_SHARED_IRQ_NON_RX;
	} else {
		trans_pcie->trans->num_rx_queues = num_irqs - 1;
	}
	WARN_ON(trans_pcie->trans->num_rx_queues > IWL_MAX_RX_HW_QUEUES);

	trans_pcie->alloc_vecs = num_irqs;
	trans_pcie->msix_enabled = true;
	return;

enable_msi:
	ret = pci_enable_msi(pdev);
	if (ret) {
		dev_err(&pdev->dev, "pci_enable_msi failed - %d\n", ret);
		/* enable rfkill interrupt: hw bug w/a */
		pci_read_config_word(pdev, PCI_COMMAND, &pci_cmd);
		if (pci_cmd & PCI_COMMAND_INTX_DISABLE) {
			pci_cmd &= ~PCI_COMMAND_INTX_DISABLE;
			pci_write_config_word(pdev, PCI_COMMAND, pci_cmd);
		}
	}
}