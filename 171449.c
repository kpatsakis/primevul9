
static void ql_get_board_info(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	u32 value;

	value = ql_read_page0_reg_l(qdev, &port_regs->portStatus);

	qdev->chip_rev_id = ((value & PORT_STATUS_REV_ID_MASK) >> 12);
	if (value & PORT_STATUS_64)
		qdev->pci_width = 64;
	else
		qdev->pci_width = 32;
	if (value & PORT_STATUS_X)
		qdev->pci_x = 1;
	else
		qdev->pci_x = 0;
	qdev->pci_slot = (u8) PCI_SLOT(qdev->pdev->devfn);