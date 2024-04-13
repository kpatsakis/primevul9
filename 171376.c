static int ql_is_auto_neg_complete(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	u32 bitToCheck = 0;
	u32 temp;

	switch (qdev->mac_index) {
	case 0:
		bitToCheck = PORT_STATUS_AC0;
		break;
	case 1:
		bitToCheck = PORT_STATUS_AC1;
		break;
	}

	temp = ql_read_page0_reg(qdev, &port_regs->portStatus);
	if (temp & bitToCheck) {
		netif_info(qdev, link, qdev->ndev, "Auto-Negotiate complete\n");
		return 1;
	}
	netif_info(qdev, link, qdev->ndev, "Auto-Negotiate incomplete\n");
	return 0;
}