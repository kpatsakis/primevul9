static int i40e_init_interrupt_scheme(struct i40e_pf *pf)
{
	int vectors = 0;
	ssize_t size;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		vectors = i40e_init_msix(pf);
		if (vectors < 0) {
			pf->flags &= ~(I40E_FLAG_MSIX_ENABLED	|
				       I40E_FLAG_IWARP_ENABLED	|
				       I40E_FLAG_RSS_ENABLED	|
				       I40E_FLAG_DCB_CAPABLE	|
				       I40E_FLAG_DCB_ENABLED	|
				       I40E_FLAG_SRIOV_ENABLED	|
				       I40E_FLAG_FD_SB_ENABLED	|
				       I40E_FLAG_FD_ATR_ENABLED	|
				       I40E_FLAG_VMDQ_ENABLED);
			pf->flags |= I40E_FLAG_FD_SB_INACTIVE;

			/* rework the queue expectations without MSIX */
			i40e_determine_queue_usage(pf);
		}
	}

	if (!(pf->flags & I40E_FLAG_MSIX_ENABLED) &&
	    (pf->flags & I40E_FLAG_MSI_ENABLED)) {
		dev_info(&pf->pdev->dev, "MSI-X not available, trying MSI\n");
		vectors = pci_enable_msi(pf->pdev);
		if (vectors < 0) {
			dev_info(&pf->pdev->dev, "MSI init failed - %d\n",
				 vectors);
			pf->flags &= ~I40E_FLAG_MSI_ENABLED;
		}
		vectors = 1;  /* one MSI or Legacy vector */
	}

	if (!(pf->flags & (I40E_FLAG_MSIX_ENABLED | I40E_FLAG_MSI_ENABLED)))
		dev_info(&pf->pdev->dev, "MSI-X and MSI not available, falling back to Legacy IRQ\n");

	/* set up vector assignment tracking */
	size = sizeof(struct i40e_lump_tracking) + (sizeof(u16) * vectors);
	pf->irq_pile = kzalloc(size, GFP_KERNEL);
	if (!pf->irq_pile)
		return -ENOMEM;

	pf->irq_pile->num_entries = vectors;
	pf->irq_pile->search_hint = 0;

	/* track first vector for misc interrupts, ignore return */
	(void)i40e_get_lump(pf, pf->irq_pile, 1, I40E_PILE_VALID_BIT - 1);

	return 0;
}