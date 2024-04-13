static void fm93c56a_deselect(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	__iomem u32 *spir = &port_regs->CommonRegs.serialPortInterfaceReg;

	qdev->eeprom_cmd_data = AUBURN_EEPROM_CS_0;
	ql_write_nvram_reg(qdev, spir, ISP_NVRAM_MASK | qdev->eeprom_cmd_data);
}