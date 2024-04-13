static int mwifiex_pcie_request_irq(struct mwifiex_adapter *adapter)
{
	int ret, i, j;
	struct pcie_service_card *card = adapter->card;
	struct pci_dev *pdev = card->dev;

	if (card->pcie.reg->msix_support) {
		for (i = 0; i < MWIFIEX_NUM_MSIX_VECTORS; i++)
			card->msix_entries[i].entry = i;
		ret = pci_enable_msix_exact(pdev, card->msix_entries,
					    MWIFIEX_NUM_MSIX_VECTORS);
		if (!ret) {
			for (i = 0; i < MWIFIEX_NUM_MSIX_VECTORS; i++) {
				card->msix_ctx[i].dev = pdev;
				card->msix_ctx[i].msg_id = i;

				ret = request_irq(card->msix_entries[i].vector,
						  mwifiex_pcie_interrupt, 0,
						  "MWIFIEX_PCIE_MSIX",
						  &card->msix_ctx[i]);
				if (ret)
					break;
			}

			if (ret) {
				mwifiex_dbg(adapter, INFO, "request_irq fail: %d\n",
					    ret);
				for (j = 0; j < i; j++)
					free_irq(card->msix_entries[j].vector,
						 &card->msix_ctx[i]);
				pci_disable_msix(pdev);
			} else {
				mwifiex_dbg(adapter, MSG, "MSIx enabled!");
				card->msix_enable = 1;
				return 0;
			}
		}
	}

	if (pci_enable_msi(pdev) != 0)
		pci_disable_msi(pdev);
	else
		card->msi_enable = 1;

	mwifiex_dbg(adapter, INFO, "msi_enable = %d\n", card->msi_enable);

	card->share_irq_ctx.dev = pdev;
	card->share_irq_ctx.msg_id = -1;
	ret = request_irq(pdev->irq, mwifiex_pcie_interrupt, IRQF_SHARED,
			  "MRVL_PCIE", &card->share_irq_ctx);
	if (ret) {
		pr_err("request_irq failed: ret=%d\n", ret);
		return -1;
	}

	return 0;
}