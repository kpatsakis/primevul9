
static void ql_reset_work(struct work_struct *work)
{
	struct ql3_adapter *qdev =
		container_of(work, struct ql3_adapter, reset_work.work);
	struct net_device *ndev = qdev->ndev;
	u32 value;
	struct ql_tx_buf_cb *tx_cb;
	int max_wait_time, i;
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	unsigned long hw_flags;

	if (test_bit((QL_RESET_PER_SCSI | QL_RESET_START), &qdev->flags)) {
		clear_bit(QL_LINK_MASTER, &qdev->flags);

		/*
		 * Loop through the active list and return the skb.
		 */
		for (i = 0; i < NUM_REQ_Q_ENTRIES; i++) {
			int j;
			tx_cb = &qdev->tx_buf[i];
			if (tx_cb->skb) {
				netdev_printk(KERN_DEBUG, ndev,
					      "Freeing lost SKB\n");
				pci_unmap_single(qdev->pdev,
					 dma_unmap_addr(&tx_cb->map[0],
							mapaddr),
					 dma_unmap_len(&tx_cb->map[0], maplen),
					 PCI_DMA_TODEVICE);
				for (j = 1; j < tx_cb->seg_count; j++) {
					pci_unmap_page(qdev->pdev,
					       dma_unmap_addr(&tx_cb->map[j],
							      mapaddr),
					       dma_unmap_len(&tx_cb->map[j],
							     maplen),
					       PCI_DMA_TODEVICE);
				}
				dev_kfree_skb(tx_cb->skb);
				tx_cb->skb = NULL;
			}
		}

		netdev_err(ndev, "Clearing NRI after reset\n");
		spin_lock_irqsave(&qdev->hw_lock, hw_flags);
		ql_write_common_reg(qdev,
				    &port_regs->CommonRegs.
				    ispControlStatus,
				    ((ISP_CONTROL_RI << 16) | ISP_CONTROL_RI));
		/*
		 * Wait the for Soft Reset to Complete.
		 */
		max_wait_time = 10;
		do {
			value = ql_read_common_reg(qdev,
						   &port_regs->CommonRegs.

						   ispControlStatus);
			if ((value & ISP_CONTROL_SR) == 0) {
				netdev_printk(KERN_DEBUG, ndev,
					      "reset completed\n");
				break;
			}

			if (value & ISP_CONTROL_RI) {
				netdev_printk(KERN_DEBUG, ndev,
					      "clearing NRI after reset\n");
				ql_write_common_reg(qdev,
						    &port_regs->
						    CommonRegs.
						    ispControlStatus,
						    ((ISP_CONTROL_RI <<
						      16) | ISP_CONTROL_RI));
			}

			spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
			ssleep(1);
			spin_lock_irqsave(&qdev->hw_lock, hw_flags);
		} while (--max_wait_time);
		spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);

		if (value & ISP_CONTROL_SR) {

			/*
			 * Set the reset flags and clear the board again.
			 * Nothing else to do...
			 */
			netdev_err(ndev,
				   "Timed out waiting for reset to complete\n");
			netdev_err(ndev, "Do a reset\n");
			clear_bit(QL_RESET_PER_SCSI, &qdev->flags);
			clear_bit(QL_RESET_START, &qdev->flags);
			ql_cycle_adapter(qdev, QL_DO_RESET);
			return;
		}

		clear_bit(QL_RESET_ACTIVE, &qdev->flags);
		clear_bit(QL_RESET_PER_SCSI, &qdev->flags);
		clear_bit(QL_RESET_START, &qdev->flags);
		ql_cycle_adapter(qdev, QL_NO_RESET);
	}