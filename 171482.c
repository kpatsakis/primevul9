static u32 ql_read_common_reg(struct ql3_adapter *qdev, u32 __iomem *reg)
{
	return readl(reg);
}