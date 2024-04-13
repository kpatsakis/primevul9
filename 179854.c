static void mwifiex_unregister_dev(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	struct pci_dev *pdev = card->dev;
	int i;

	if (card->msix_enable) {
		for (i = 0; i < MWIFIEX_NUM_MSIX_VECTORS; i++)
			synchronize_irq(card->msix_entries[i].vector);

		for (i = 0; i < MWIFIEX_NUM_MSIX_VECTORS; i++)
			free_irq(card->msix_entries[i].vector,
				 &card->msix_ctx[i]);

		card->msix_enable = 0;
		pci_disable_msix(pdev);
	} else {
		mwifiex_dbg(adapter, INFO,
			    "%s(): calling free_irq()\n", __func__);
	       free_irq(card->dev->irq, &card->share_irq_ctx);

		if (card->msi_enable)
			pci_disable_msi(pdev);
	}
	card->adapter = NULL;
}