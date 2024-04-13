static void i40e_pci_error_reset_prepare(struct pci_dev *pdev)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);

	i40e_prep_for_reset(pf, false);
}