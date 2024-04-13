static int mwifiex_pcie_probe(struct pci_dev *pdev,
					const struct pci_device_id *ent)
{
	struct pcie_service_card *card;
	int ret;

	pr_debug("info: vendor=0x%4.04X device=0x%4.04X rev=%d\n",
		 pdev->vendor, pdev->device, pdev->revision);

	card = devm_kzalloc(&pdev->dev, sizeof(*card), GFP_KERNEL);
	if (!card)
		return -ENOMEM;

	init_completion(&card->fw_done);

	card->dev = pdev;

	if (ent->driver_data) {
		struct mwifiex_pcie_device *data = (void *)ent->driver_data;
		card->pcie.reg = data->reg;
		card->pcie.blksz_fw_dl = data->blksz_fw_dl;
		card->pcie.tx_buf_size = data->tx_buf_size;
		card->pcie.can_dump_fw = data->can_dump_fw;
		card->pcie.mem_type_mapping_tbl = data->mem_type_mapping_tbl;
		card->pcie.num_mem_types = data->num_mem_types;
		card->pcie.can_ext_scan = data->can_ext_scan;
		INIT_WORK(&card->work, mwifiex_pcie_work);
	}

	/* device tree node parsing and platform specific configuration*/
	if (pdev->dev.of_node) {
		ret = mwifiex_pcie_probe_of(&pdev->dev);
		if (ret)
			return ret;
	}

	if (mwifiex_add_card(card, &card->fw_done, &pcie_ops,
			     MWIFIEX_PCIE, &pdev->dev)) {
		pr_err("%s failed\n", __func__);
		return -1;
	}

	return 0;
}