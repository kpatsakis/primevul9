static void phyAgereSpecificInit(struct ql3_adapter *qdev, u32 miiAddr)
{
	netdev_info(qdev->ndev, "enabling Agere specific PHY\n");
	/* power down device bit 11 = 1 */
	ql_mii_write_reg_ex(qdev, 0x00, 0x1940, miiAddr);
	/* enable diagnostic mode bit 2 = 1 */
	ql_mii_write_reg_ex(qdev, 0x12, 0x840e, miiAddr);
	/* 1000MB amplitude adjust (see Agere errata) */
	ql_mii_write_reg_ex(qdev, 0x10, 0x8805, miiAddr);
	/* 1000MB amplitude adjust (see Agere errata) */
	ql_mii_write_reg_ex(qdev, 0x11, 0xf03e, miiAddr);
	/* 100MB amplitude adjust (see Agere errata) */
	ql_mii_write_reg_ex(qdev, 0x10, 0x8806, miiAddr);
	/* 100MB amplitude adjust (see Agere errata) */
	ql_mii_write_reg_ex(qdev, 0x11, 0x003e, miiAddr);
	/* 10MB amplitude adjust (see Agere errata) */
	ql_mii_write_reg_ex(qdev, 0x10, 0x8807, miiAddr);
	/* 10MB amplitude adjust (see Agere errata) */
	ql_mii_write_reg_ex(qdev, 0x11, 0x1f00, miiAddr);
	/* point to hidden reg 0x2806 */
	ql_mii_write_reg_ex(qdev, 0x10, 0x2806, miiAddr);
	/* Write new PHYAD w/bit 5 set */
	ql_mii_write_reg_ex(qdev, 0x11,
			    0x0020 | (PHYAddr[qdev->mac_index] >> 8), miiAddr);
	/*
	 * Disable diagnostic mode bit 2 = 0
	 * Power up device bit 11 = 0
	 * Link up (on) and activity (blink)
	 */
	ql_mii_write_reg(qdev, 0x12, 0x840a);
	ql_mii_write_reg(qdev, 0x00, 0x1140);
	ql_mii_write_reg(qdev, 0x1c, 0xfaf0);
}