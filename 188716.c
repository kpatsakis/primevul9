static pci_ers_result_t i40e_pci_error_detected(struct pci_dev *pdev,
						enum pci_channel_state error)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s: error %d\n", __func__, error);

	if (!pf) {
		dev_info(&pdev->dev,
			 "Cannot recover - error happened during device probe\n");
		return PCI_ERS_RESULT_DISCONNECT;
	}

	/* shutdown all operations */
	if (!test_bit(__I40E_SUSPENDED, pf->state))
		i40e_prep_for_reset(pf, false);

	/* Request a slot reset */
	return PCI_ERS_RESULT_NEED_RESET;
}