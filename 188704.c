static int i40e_reserve_msix_vectors(struct i40e_pf *pf, int vectors)
{
	vectors = pci_enable_msix_range(pf->pdev, pf->msix_entries,
					I40E_MIN_MSIX, vectors);
	if (vectors < 0) {
		dev_info(&pf->pdev->dev,
			 "MSI-X vector reservation failed: %d\n", vectors);
		vectors = 0;
	}

	return vectors;
}