void iwl_pcie_synchronize_irqs(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (trans_pcie->msix_enabled) {
		int i;

		for (i = 0; i < trans_pcie->alloc_vecs; i++)
			synchronize_irq(trans_pcie->msix_entries[i].vector);
	} else {
		synchronize_irq(trans_pcie->pci_dev->irq);
	}
}