static int ql_this_adapter_controls_port(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 bitToCheck = 0;
	u32 temp;

	switch (qdev->mac_index) {
	case 0:
		bitToCheck = PORT_STATUS_F1_ENABLED;
		break;
	case 1:
		bitToCheck = PORT_STATUS_F3_ENABLED;
		break;
	default:
		break;
	}

	temp = ql_read_page0_reg(qdev, &port_regs->portStatus);
	if (temp & bitToCheck) {
		netif_printk(qdev, link, KERN_DEBUG, qdev->ndev,
			     "not link master\n");
		return 0;
	}

	netif_printk(qdev, link, KERN_DEBUG, qdev->ndev, "link master\n");
	return 1;
}