static void i40e_pci_error_reset_done(struct pci_dev *pdev)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);

	i40e_reset_and_rebuild(pf, false, false);
}