
static void ql_set_mac_info(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	u32 value, port_status;
	u8 func_number;

	/* Get the function number */
	value =
	    ql_read_common_reg_l(qdev, &port_regs->CommonRegs.ispControlStatus);
	func_number = (u8) ((value >> 4) & OPCODE_FUNC_ID_MASK);
	port_status = ql_read_page0_reg(qdev, &port_regs->portStatus);
	switch (value & ISP_CONTROL_FN_MASK) {
	case ISP_CONTROL_FN0_NET:
		qdev->mac_index = 0;
		qdev->mac_ob_opcode = OUTBOUND_MAC_IOCB | func_number;
		qdev->mb_bit_mask = FN0_MA_BITS_MASK;
		qdev->PHYAddr = PORT0_PHY_ADDRESS;
		if (port_status & PORT_STATUS_SM0)
			set_bit(QL_LINK_OPTICAL, &qdev->flags);
		else
			clear_bit(QL_LINK_OPTICAL, &qdev->flags);
		break;

	case ISP_CONTROL_FN1_NET:
		qdev->mac_index = 1;
		qdev->mac_ob_opcode = OUTBOUND_MAC_IOCB | func_number;
		qdev->mb_bit_mask = FN1_MA_BITS_MASK;
		qdev->PHYAddr = PORT1_PHY_ADDRESS;
		if (port_status & PORT_STATUS_SM1)
			set_bit(QL_LINK_OPTICAL, &qdev->flags);
		else
			clear_bit(QL_LINK_OPTICAL, &qdev->flags);
		break;

	case ISP_CONTROL_FN0_SCSI:
	case ISP_CONTROL_FN1_SCSI:
	default:
		netdev_printk(KERN_DEBUG, qdev->ndev,
			      "Invalid function number, ispControlStatus = 0x%x\n",
			      value);
		break;
	}
	qdev->numPorts = qdev->nvram_data.version_and_numPorts >> 8;