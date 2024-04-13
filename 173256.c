static bool bnx2x_prev_unload_close_umac(struct bnx2x *bp,
					 u8 port, u32 reset_reg,
					 struct bnx2x_mac_vals *vals)
{
	u32 mask = MISC_REGISTERS_RESET_REG_2_UMAC0 << port;
	u32 base_addr;

	if (!(mask & reset_reg))
		return false;

	BNX2X_DEV_INFO("Disable umac Rx %02x\n", port);
	base_addr = port ? GRCBASE_UMAC1 : GRCBASE_UMAC0;
	vals->umac_addr[port] = base_addr + UMAC_REG_COMMAND_CONFIG;
	vals->umac_val[port] = REG_RD(bp, vals->umac_addr[port]);
	REG_WR(bp, vals->umac_addr[port], 0);

	return true;
}