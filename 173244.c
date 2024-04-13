static void bnx2x_prev_unload_close_mac(struct bnx2x *bp,
					struct bnx2x_mac_vals *vals)
{
	u32 val, base_addr, offset, mask, reset_reg;
	bool mac_stopped = false;
	u8 port = BP_PORT(bp);

	/* reset addresses as they also mark which values were changed */
	memset(vals, 0, sizeof(*vals));

	reset_reg = REG_RD(bp, MISC_REG_RESET_REG_2);

	if (!CHIP_IS_E3(bp)) {
		val = REG_RD(bp, NIG_REG_BMAC0_REGS_OUT_EN + port * 4);
		mask = MISC_REGISTERS_RESET_REG_2_RST_BMAC0 << port;
		if ((mask & reset_reg) && val) {
			u32 wb_data[2];
			BNX2X_DEV_INFO("Disable bmac Rx\n");
			base_addr = BP_PORT(bp) ? NIG_REG_INGRESS_BMAC1_MEM
						: NIG_REG_INGRESS_BMAC0_MEM;
			offset = CHIP_IS_E2(bp) ? BIGMAC2_REGISTER_BMAC_CONTROL
						: BIGMAC_REGISTER_BMAC_CONTROL;

			/*
			 * use rd/wr since we cannot use dmae. This is safe
			 * since MCP won't access the bus due to the request
			 * to unload, and no function on the path can be
			 * loaded at this time.
			 */
			wb_data[0] = REG_RD(bp, base_addr + offset);
			wb_data[1] = REG_RD(bp, base_addr + offset + 0x4);
			vals->bmac_addr = base_addr + offset;
			vals->bmac_val[0] = wb_data[0];
			vals->bmac_val[1] = wb_data[1];
			wb_data[0] &= ~BMAC_CONTROL_RX_ENABLE;
			REG_WR(bp, vals->bmac_addr, wb_data[0]);
			REG_WR(bp, vals->bmac_addr + 0x4, wb_data[1]);
		}
		BNX2X_DEV_INFO("Disable emac Rx\n");
		vals->emac_addr = NIG_REG_NIG_EMAC0_EN + BP_PORT(bp)*4;
		vals->emac_val = REG_RD(bp, vals->emac_addr);
		REG_WR(bp, vals->emac_addr, 0);
		mac_stopped = true;
	} else {
		if (reset_reg & MISC_REGISTERS_RESET_REG_2_XMAC) {
			BNX2X_DEV_INFO("Disable xmac Rx\n");
			base_addr = BP_PORT(bp) ? GRCBASE_XMAC1 : GRCBASE_XMAC0;
			val = REG_RD(bp, base_addr + XMAC_REG_PFC_CTRL_HI);
			REG_WR(bp, base_addr + XMAC_REG_PFC_CTRL_HI,
			       val & ~(1 << 1));
			REG_WR(bp, base_addr + XMAC_REG_PFC_CTRL_HI,
			       val | (1 << 1));
			vals->xmac_addr = base_addr + XMAC_REG_CTRL;
			vals->xmac_val = REG_RD(bp, vals->xmac_addr);
			REG_WR(bp, vals->xmac_addr, 0);
			mac_stopped = true;
		}

		mac_stopped |= bnx2x_prev_unload_close_umac(bp, 0,
							    reset_reg, vals);
		mac_stopped |= bnx2x_prev_unload_close_umac(bp, 1,
							    reset_reg, vals);
	}

	if (mac_stopped)
		msleep(20);
}