static pci_ers_result_t i40e_pci_error_slot_reset(struct pci_dev *pdev)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);
	pci_ers_result_t result;
	u32 reg;

	dev_dbg(&pdev->dev, "%s\n", __func__);
	if (pci_enable_device_mem(pdev)) {
		dev_info(&pdev->dev,
			 "Cannot re-enable PCI device after reset.\n");
		result = PCI_ERS_RESULT_DISCONNECT;
	} else {
		pci_set_master(pdev);
		pci_restore_state(pdev);
		pci_save_state(pdev);
		pci_wake_from_d3(pdev, false);

		reg = rd32(&pf->hw, I40E_GLGEN_RTRIG);
		if (reg == 0)
			result = PCI_ERS_RESULT_RECOVERED;
		else
			result = PCI_ERS_RESULT_DISCONNECT;
	}

	return result;
}