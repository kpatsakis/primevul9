
static int ql_adapter_up(struct ql3_adapter *qdev)
{
	struct net_device *ndev = qdev->ndev;
	int err;
	unsigned long irq_flags = IRQF_SHARED;
	unsigned long hw_flags;

	if (ql_alloc_mem_resources(qdev)) {
		netdev_err(ndev, "Unable to  allocate buffers\n");
		return -ENOMEM;
	}

	if (qdev->msi) {
		if (pci_enable_msi(qdev->pdev)) {
			netdev_err(ndev,
				   "User requested MSI, but MSI failed to initialize.  Continuing without MSI.\n");
			qdev->msi = 0;
		} else {
			netdev_info(ndev, "MSI Enabled...\n");
			set_bit(QL_MSI_ENABLED, &qdev->flags);
			irq_flags &= ~IRQF_SHARED;
		}
	}

	err = request_irq(qdev->pdev->irq, ql3xxx_isr,
			  irq_flags, ndev->name, ndev);
	if (err) {
		netdev_err(ndev,
			   "Failed to reserve interrupt %d - already in use\n",
			   qdev->pdev->irq);
		goto err_irq;
	}

	spin_lock_irqsave(&qdev->hw_lock, hw_flags);

	err = ql_wait_for_drvr_lock(qdev);
	if (err) {
		err = ql_adapter_initialize(qdev);
		if (err) {
			netdev_err(ndev, "Unable to initialize adapter\n");
			goto err_init;
		}
		netdev_err(ndev, "Releasing driver lock\n");
		ql_sem_unlock(qdev, QL_DRVR_SEM_MASK);
	} else {
		netdev_err(ndev, "Could not acquire driver lock\n");
		goto err_lock;
	}

	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);

	set_bit(QL_ADAPTER_UP, &qdev->flags);

	mod_timer(&qdev->adapter_timer, jiffies + HZ * 1);

	napi_enable(&qdev->napi);
	ql_enable_interrupts(qdev);
	return 0;

err_init:
	ql_sem_unlock(qdev, QL_DRVR_SEM_MASK);
err_lock:
	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
	free_irq(qdev->pdev->irq, ndev);
err_irq:
	if (qdev->msi && test_bit(QL_MSI_ENABLED, &qdev->flags)) {
		netdev_info(ndev, "calling pci_disable_msi()\n");
		clear_bit(QL_MSI_ENABLED, &qdev->flags);
		pci_disable_msi(qdev->pdev);
	}
	return err;