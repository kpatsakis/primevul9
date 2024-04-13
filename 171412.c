static int ql_is_auto_cfg(struct ql3_adapter *qdev)
{
	u16 reg;
	ql_mii_read_reg(qdev, 0x00, &reg);
	return (reg & 0x1000) != 0;
}