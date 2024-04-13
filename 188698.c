static void i40e_reset_interrupt_capability(struct i40e_pf *pf)
{
	/* If we're in Legacy mode, the interrupt was cleaned in vsi_close */
	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		pci_disable_msix(pf->pdev);
		kfree(pf->msix_entries);
		pf->msix_entries = NULL;
		kfree(pf->irq_pile);
		pf->irq_pile = NULL;
	} else if (pf->flags & I40E_FLAG_MSI_ENABLED) {
		pci_disable_msi(pf->pdev);
	}
	pf->flags &= ~(I40E_FLAG_MSIX_ENABLED | I40E_FLAG_MSI_ENABLED);
}