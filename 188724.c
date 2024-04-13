static void i40e_pci_error_resume(struct pci_dev *pdev)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);

	dev_dbg(&pdev->dev, "%s\n", __func__);
	if (test_bit(__I40E_SUSPENDED, pf->state))
		return;

	i40e_handle_reset_warning(pf, false);
}