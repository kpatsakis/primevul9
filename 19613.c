void iwl_pcie_dump_csr(struct iwl_trans *trans)
{
	int i;
	static const u32 csr_tbl[] = {
		CSR_HW_IF_CONFIG_REG,
		CSR_INT_COALESCING,
		CSR_INT,
		CSR_INT_MASK,
		CSR_FH_INT_STATUS,
		CSR_GPIO_IN,
		CSR_RESET,
		CSR_GP_CNTRL,
		CSR_HW_REV,
		CSR_EEPROM_REG,
		CSR_EEPROM_GP,
		CSR_OTP_GP_REG,
		CSR_GIO_REG,
		CSR_GP_UCODE_REG,
		CSR_GP_DRIVER_REG,
		CSR_UCODE_DRV_GP1,
		CSR_UCODE_DRV_GP2,
		CSR_LED_REG,
		CSR_DRAM_INT_TBL_REG,
		CSR_GIO_CHICKEN_BITS,
		CSR_ANA_PLL_CFG,
		CSR_MONITOR_STATUS_REG,
		CSR_HW_REV_WA_REG,
		CSR_DBG_HPET_MEM_REG
	};
	IWL_ERR(trans, "CSR values:\n");
	IWL_ERR(trans, "(2nd byte of CSR_INT_COALESCING is "
		"CSR_INT_PERIODIC_REG)\n");
	for (i = 0; i <  ARRAY_SIZE(csr_tbl); i++) {
		IWL_ERR(trans, "  %25s: 0X%08x\n",
			get_csr_string(csr_tbl[i]),
			iwl_read32(trans, csr_tbl[i]));
	}
}