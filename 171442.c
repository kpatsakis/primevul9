static void fm93c56a_cmd(struct ql3_adapter *qdev, u32 cmd, u32 eepromAddr)
{
	int i;
	u32 mask;
	u32 dataBit;
	u32 previousBit;
	struct ql3xxx_port_registers __iomem *port_regs =
			qdev->mem_map_registers;
	__iomem u32 *spir = &port_regs->CommonRegs.serialPortInterfaceReg;

	/* Clock in a zero, then do the start bit */
	ql_write_nvram_reg(qdev, spir,
			   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
			    AUBURN_EEPROM_DO_1));
	ql_write_nvram_reg(qdev, spir,
			   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
			    AUBURN_EEPROM_DO_1 | AUBURN_EEPROM_CLK_RISE));
	ql_write_nvram_reg(qdev, spir,
			   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
			    AUBURN_EEPROM_DO_1 | AUBURN_EEPROM_CLK_FALL));

	mask = 1 << (FM93C56A_CMD_BITS - 1);
	/* Force the previous data bit to be different */
	previousBit = 0xffff;
	for (i = 0; i < FM93C56A_CMD_BITS; i++) {
		dataBit = (cmd & mask)
			? AUBURN_EEPROM_DO_1
			: AUBURN_EEPROM_DO_0;
		if (previousBit != dataBit) {
			/* If the bit changed, change the DO state to match */
			ql_write_nvram_reg(qdev, spir,
					   (ISP_NVRAM_MASK |
					    qdev->eeprom_cmd_data | dataBit));
			previousBit = dataBit;
		}
		ql_write_nvram_reg(qdev, spir,
				   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
				    dataBit | AUBURN_EEPROM_CLK_RISE));
		ql_write_nvram_reg(qdev, spir,
				   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
				    dataBit | AUBURN_EEPROM_CLK_FALL));
		cmd = cmd << 1;
	}

	mask = 1 << (addrBits - 1);
	/* Force the previous data bit to be different */
	previousBit = 0xffff;
	for (i = 0; i < addrBits; i++) {
		dataBit = (eepromAddr & mask) ? AUBURN_EEPROM_DO_1
			: AUBURN_EEPROM_DO_0;
		if (previousBit != dataBit) {
			/*
			 * If the bit changed, then change the DO state to
			 * match
			 */
			ql_write_nvram_reg(qdev, spir,
					   (ISP_NVRAM_MASK |
					    qdev->eeprom_cmd_data | dataBit));
			previousBit = dataBit;
		}
		ql_write_nvram_reg(qdev, spir,
				   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
				    dataBit | AUBURN_EEPROM_CLK_RISE));
		ql_write_nvram_reg(qdev, spir,
				   (ISP_NVRAM_MASK | qdev->eeprom_cmd_data |
				    dataBit | AUBURN_EEPROM_CLK_FALL));
		eepromAddr = eepromAddr << 1;
	}
}