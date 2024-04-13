static void fm93c56a_datain(struct ql3_adapter *qdev, unsigned short *value)
{
	int i;
	u32 data = 0;
	u32 dataBit;
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	__iomem u32 *spir = &port_regs->CommonRegs.serialPortInterfaceReg;

	/* Read the data bits */
	/* The first bit is a dummy.  Clock right over it. */
	for (i = 0; i < dataBits; i++) {
		ql_write_nvram_reg(qdev, spir,
				   ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
				   AUBURN_EEPROM_CLK_RISE);
		ql_write_nvram_reg(qdev, spir,
				   ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
				   AUBURN_EEPROM_CLK_FALL);
		dataBit = (ql_read_common_reg(qdev, spir) &
			   AUBURN_EEPROM_DI_1) ? 1 : 0;
		data = (data << 1) | dataBit;
	}
	*value = (u16)data;
}