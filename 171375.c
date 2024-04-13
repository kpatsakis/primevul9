
static int ql3xxx_probe(struct pci_dev *pdev,
			const struct pci_device_id *pci_entry)
{
	struct net_device *ndev = NULL;
	struct ql3_adapter *qdev = NULL;
	static int cards_found;
	int uninitialized_var(pci_using_dac), err;

	err = pci_enable_device(pdev);
	if (err) {
		pr_err("%s cannot enable PCI device\n", pci_name(pdev));
		goto err_out;
	}

	err = pci_request_regions(pdev, DRV_NAME);
	if (err) {
		pr_err("%s cannot obtain PCI resources\n", pci_name(pdev));
		goto err_out_disable_pdev;
	}

	pci_set_master(pdev);

	if (!pci_set_dma_mask(pdev, DMA_BIT_MASK(64))) {
		pci_using_dac = 1;
		err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(64));
	} else if (!(err = pci_set_dma_mask(pdev, DMA_BIT_MASK(32)))) {
		pci_using_dac = 0;
		err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
	}

	if (err) {
		pr_err("%s no usable DMA configuration\n", pci_name(pdev));
		goto err_out_free_regions;
	}

	ndev = alloc_etherdev(sizeof(struct ql3_adapter));
	if (!ndev) {
		err = -ENOMEM;
		goto err_out_free_regions;
	}

	SET_NETDEV_DEV(ndev, &pdev->dev);

	pci_set_drvdata(pdev, ndev);

	qdev = netdev_priv(ndev);
	qdev->index = cards_found;
	qdev->ndev = ndev;
	qdev->pdev = pdev;
	qdev->device_id = pci_entry->device;
	qdev->port_link_state = LS_DOWN;
	if (msi)
		qdev->msi = 1;

	qdev->msg_enable = netif_msg_init(debug, default_msg);

	if (pci_using_dac)
		ndev->features |= NETIF_F_HIGHDMA;
	if (qdev->device_id == QL3032_DEVICE_ID)
		ndev->features |= NETIF_F_IP_CSUM | NETIF_F_SG;

	qdev->mem_map_registers = pci_ioremap_bar(pdev, 1);
	if (!qdev->mem_map_registers) {
		pr_err("%s: cannot map device registers\n", pci_name(pdev));
		err = -EIO;
		goto err_out_free_ndev;
	}

	spin_lock_init(&qdev->adapter_lock);
	spin_lock_init(&qdev->hw_lock);

	/* Set driver entry points */
	ndev->netdev_ops = &ql3xxx_netdev_ops;
	ndev->ethtool_ops = &ql3xxx_ethtool_ops;
	ndev->watchdog_timeo = 5 * HZ;

	netif_napi_add(ndev, &qdev->napi, ql_poll, 64);

	ndev->irq = pdev->irq;

	/* make sure the EEPROM is good */
	if (ql_get_nvram_params(qdev)) {
		pr_alert("%s: Adapter #%d, Invalid NVRAM parameters\n",
			 __func__, qdev->index);
		err = -EIO;
		goto err_out_iounmap;
	}

	ql_set_mac_info(qdev);

	/* Validate and set parameters */
	if (qdev->mac_index) {
		ndev->mtu = qdev->nvram_data.macCfg_port1.etherMtu_mac ;
		ql_set_mac_addr(ndev, qdev->nvram_data.funcCfg_fn2.macAddress);
	} else {
		ndev->mtu = qdev->nvram_data.macCfg_port0.etherMtu_mac ;
		ql_set_mac_addr(ndev, qdev->nvram_data.funcCfg_fn0.macAddress);
	}

	ndev->tx_queue_len = NUM_REQ_Q_ENTRIES;

	/* Record PCI bus information. */
	ql_get_board_info(qdev);

	/*
	 * Set the Maximum Memory Read Byte Count value. We do this to handle
	 * jumbo frames.
	 */
	if (qdev->pci_x)
		pci_write_config_word(pdev, (int)0x4e, (u16) 0x0036);

	err = register_netdev(ndev);
	if (err) {
		pr_err("%s: cannot register net device\n", pci_name(pdev));
		goto err_out_iounmap;
	}

	/* we're going to reset, so assume we have no link for now */

	netif_carrier_off(ndev);
	netif_stop_queue(ndev);

	qdev->workqueue = create_singlethread_workqueue(ndev->name);
	if (!qdev->workqueue) {
		unregister_netdev(ndev);
		err = -ENOMEM;
		goto err_out_iounmap;
	}

	INIT_DELAYED_WORK(&qdev->reset_work, ql_reset_work);
	INIT_DELAYED_WORK(&qdev->tx_timeout_work, ql_tx_timeout_work);
	INIT_DELAYED_WORK(&qdev->link_state_work, ql_link_state_machine_work);

	timer_setup(&qdev->adapter_timer, ql3xxx_timer, 0);
	qdev->adapter_timer.expires = jiffies + HZ * 2;	/* two second delay */

	if (!cards_found) {
		pr_alert("%s\n", DRV_STRING);
		pr_alert("Driver name: %s, Version: %s\n",
			 DRV_NAME, DRV_VERSION);
	}
	ql_display_dev_info(ndev);

	cards_found++;
	return 0;

err_out_iounmap:
	iounmap(qdev->mem_map_registers);
err_out_free_ndev:
	free_netdev(ndev);
err_out_free_regions:
	pci_release_regions(pdev);
err_out_disable_pdev:
	pci_disable_device(pdev);
err_out:
	return err;