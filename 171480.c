
static int ql_adapter_initialize(struct ql3_adapter *qdev)
{
	u32 value;
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	__iomem u32 *spir = &port_regs->CommonRegs.serialPortInterfaceReg;
	struct ql3xxx_host_memory_registers __iomem *hmem_regs =
		(void __iomem *)port_regs;
	u32 delay = 10;
	int status = 0;

	if (ql_mii_setup(qdev))
		return -1;

	/* Bring out PHY out of reset */
	ql_write_common_reg(qdev, spir,
			    (ISP_SERIAL_PORT_IF_WE |
			     (ISP_SERIAL_PORT_IF_WE << 16)));
	/* Give the PHY time to come out of reset. */
	mdelay(100);
	qdev->port_link_state = LS_DOWN;
	netif_carrier_off(qdev->ndev);

	/* V2 chip fix for ARS-39168. */
	ql_write_common_reg(qdev, spir,
			    (ISP_SERIAL_PORT_IF_SDE |
			     (ISP_SERIAL_PORT_IF_SDE << 16)));

	/* Request Queue Registers */
	*((u32 *)(qdev->preq_consumer_index)) = 0;
	atomic_set(&qdev->tx_count, NUM_REQ_Q_ENTRIES);
	qdev->req_producer_index = 0;

	ql_write_page1_reg(qdev,
			   &hmem_regs->reqConsumerIndexAddrHigh,
			   qdev->req_consumer_index_phy_addr_high);
	ql_write_page1_reg(qdev,
			   &hmem_regs->reqConsumerIndexAddrLow,
			   qdev->req_consumer_index_phy_addr_low);

	ql_write_page1_reg(qdev,
			   &hmem_regs->reqBaseAddrHigh,
			   MS_64BITS(qdev->req_q_phy_addr));
	ql_write_page1_reg(qdev,
			   &hmem_regs->reqBaseAddrLow,
			   LS_64BITS(qdev->req_q_phy_addr));
	ql_write_page1_reg(qdev, &hmem_regs->reqLength, NUM_REQ_Q_ENTRIES);

	/* Response Queue Registers */
	*((__le16 *) (qdev->prsp_producer_index)) = 0;
	qdev->rsp_consumer_index = 0;
	qdev->rsp_current = qdev->rsp_q_virt_addr;

	ql_write_page1_reg(qdev,
			   &hmem_regs->rspProducerIndexAddrHigh,
			   qdev->rsp_producer_index_phy_addr_high);

	ql_write_page1_reg(qdev,
			   &hmem_regs->rspProducerIndexAddrLow,
			   qdev->rsp_producer_index_phy_addr_low);

	ql_write_page1_reg(qdev,
			   &hmem_regs->rspBaseAddrHigh,
			   MS_64BITS(qdev->rsp_q_phy_addr));

	ql_write_page1_reg(qdev,
			   &hmem_regs->rspBaseAddrLow,
			   LS_64BITS(qdev->rsp_q_phy_addr));

	ql_write_page1_reg(qdev, &hmem_regs->rspLength, NUM_RSP_Q_ENTRIES);

	/* Large Buffer Queue */
	ql_write_page1_reg(qdev,
			   &hmem_regs->rxLargeQBaseAddrHigh,
			   MS_64BITS(qdev->lrg_buf_q_phy_addr));

	ql_write_page1_reg(qdev,
			   &hmem_regs->rxLargeQBaseAddrLow,
			   LS_64BITS(qdev->lrg_buf_q_phy_addr));

	ql_write_page1_reg(qdev,
			   &hmem_regs->rxLargeQLength,
			   qdev->num_lbufq_entries);

	ql_write_page1_reg(qdev,
			   &hmem_regs->rxLargeBufferLength,
			   qdev->lrg_buffer_len);

	/* Small Buffer Queue */
	ql_write_page1_reg(qdev,
			   &hmem_regs->rxSmallQBaseAddrHigh,
			   MS_64BITS(qdev->small_buf_q_phy_addr));

	ql_write_page1_reg(qdev,
			   &hmem_regs->rxSmallQBaseAddrLow,
			   LS_64BITS(qdev->small_buf_q_phy_addr));

	ql_write_page1_reg(qdev, &hmem_regs->rxSmallQLength, NUM_SBUFQ_ENTRIES);
	ql_write_page1_reg(qdev,
			   &hmem_regs->rxSmallBufferLength,
			   QL_SMALL_BUFFER_SIZE);

	qdev->small_buf_q_producer_index = NUM_SBUFQ_ENTRIES - 1;
	qdev->small_buf_release_cnt = 8;
	qdev->lrg_buf_q_producer_index = qdev->num_lbufq_entries - 1;
	qdev->lrg_buf_release_cnt = 8;
	qdev->lrg_buf_next_free = qdev->lrg_buf_q_virt_addr;
	qdev->small_buf_index = 0;
	qdev->lrg_buf_index = 0;
	qdev->lrg_buf_free_count = 0;
	qdev->lrg_buf_free_head = NULL;
	qdev->lrg_buf_free_tail = NULL;

	ql_write_common_reg(qdev,
			    &port_regs->CommonRegs.
			    rxSmallQProducerIndex,
			    qdev->small_buf_q_producer_index);
	ql_write_common_reg(qdev,
			    &port_regs->CommonRegs.
			    rxLargeQProducerIndex,
			    qdev->lrg_buf_q_producer_index);

	/*
	 * Find out if the chip has already been initialized.  If it has, then
	 * we skip some of the initialization.
	 */
	clear_bit(QL_LINK_MASTER, &qdev->flags);
	value = ql_read_page0_reg(qdev, &port_regs->portStatus);
	if ((value & PORT_STATUS_IC) == 0) {

		/* Chip has not been configured yet, so let it rip. */
		if (ql_init_misc_registers(qdev)) {
			status = -1;
			goto out;
		}

		value = qdev->nvram_data.tcpMaxWindowSize;
		ql_write_page0_reg(qdev, &port_regs->tcpMaxWindow, value);

		value = (0xFFFF << 16) | qdev->nvram_data.extHwConfig;

		if (ql_sem_spinlock(qdev, QL_FLASH_SEM_MASK,
				(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index)
				 * 2) << 13)) {
			status = -1;
			goto out;
		}
		ql_write_page0_reg(qdev, &port_regs->ExternalHWConfig, value);
		ql_write_page0_reg(qdev, &port_regs->InternalChipConfig,
				   (((INTERNAL_CHIP_SD | INTERNAL_CHIP_WE) <<
				     16) | (INTERNAL_CHIP_SD |
					    INTERNAL_CHIP_WE)));
		ql_sem_unlock(qdev, QL_FLASH_SEM_MASK);
	}

	if (qdev->mac_index)
		ql_write_page0_reg(qdev,
				   &port_regs->mac1MaxFrameLengthReg,
				   qdev->max_frame_size);
	else
		ql_write_page0_reg(qdev,
					   &port_regs->mac0MaxFrameLengthReg,
					   qdev->max_frame_size);

	if (ql_sem_spinlock(qdev, QL_PHY_GIO_SEM_MASK,
			(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index) *
			 2) << 7)) {
		status = -1;
		goto out;
	}

	PHY_Setup(qdev);
	ql_init_scan_mode(qdev);
	ql_get_phy_owner(qdev);

	/* Load the MAC Configuration */

	/* Program lower 32 bits of the MAC address */
	ql_write_page0_reg(qdev, &port_regs->macAddrIndirectPtrReg,
			   (MAC_ADDR_INDIRECT_PTR_REG_RP_MASK << 16));
	ql_write_page0_reg(qdev, &port_regs->macAddrDataReg,
			   ((qdev->ndev->dev_addr[2] << 24)
			    | (qdev->ndev->dev_addr[3] << 16)
			    | (qdev->ndev->dev_addr[4] << 8)
			    | qdev->ndev->dev_addr[5]));

	/* Program top 16 bits of the MAC address */
	ql_write_page0_reg(qdev, &port_regs->macAddrIndirectPtrReg,
			   ((MAC_ADDR_INDIRECT_PTR_REG_RP_MASK << 16) | 1));
	ql_write_page0_reg(qdev, &port_regs->macAddrDataReg,
			   ((qdev->ndev->dev_addr[0] << 8)
			    | qdev->ndev->dev_addr[1]));

	/* Enable Primary MAC */
	ql_write_page0_reg(qdev, &port_regs->macAddrIndirectPtrReg,
			   ((MAC_ADDR_INDIRECT_PTR_REG_PE << 16) |
			    MAC_ADDR_INDIRECT_PTR_REG_PE));

	/* Clear Primary and Secondary IP addresses */
	ql_write_page0_reg(qdev, &port_regs->ipAddrIndexReg,
			   ((IP_ADDR_INDEX_REG_MASK << 16) |
			    (qdev->mac_index << 2)));
	ql_write_page0_reg(qdev, &port_regs->ipAddrDataReg, 0);

	ql_write_page0_reg(qdev, &port_regs->ipAddrIndexReg,
			   ((IP_ADDR_INDEX_REG_MASK << 16) |
			    ((qdev->mac_index << 2) + 1)));
	ql_write_page0_reg(qdev, &port_regs->ipAddrDataReg, 0);

	ql_sem_unlock(qdev, QL_PHY_GIO_SEM_MASK);

	/* Indicate Configuration Complete */
	ql_write_page0_reg(qdev,
			   &port_regs->portControl,
			   ((PORT_CONTROL_CC << 16) | PORT_CONTROL_CC));

	do {
		value = ql_read_page0_reg(qdev, &port_regs->portStatus);
		if (value & PORT_STATUS_IC)
			break;
		spin_unlock_irq(&qdev->hw_lock);
		msleep(500);
		spin_lock_irq(&qdev->hw_lock);
	} while (--delay);

	if (delay == 0) {
		netdev_err(qdev->ndev, "Hw Initialization timeout\n");
		status = -1;
		goto out;
	}

	/* Enable Ethernet Function */
	if (qdev->device_id == QL3032_DEVICE_ID) {
		value =
		    (QL3032_PORT_CONTROL_EF | QL3032_PORT_CONTROL_KIE |
		     QL3032_PORT_CONTROL_EIv6 | QL3032_PORT_CONTROL_EIv4 |
			QL3032_PORT_CONTROL_ET);
		ql_write_page0_reg(qdev, &port_regs->functionControl,
				   ((value << 16) | value));
	} else {
		value =
		    (PORT_CONTROL_EF | PORT_CONTROL_ET | PORT_CONTROL_EI |
		     PORT_CONTROL_HH);
		ql_write_page0_reg(qdev, &port_regs->portControl,
				   ((value << 16) | value));
	}


out:
	return status;