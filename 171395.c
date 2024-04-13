static void ql_write_nvram_reg(struct ql3_adapter *qdev,
				u32 __iomem *reg, u32 value)
{
	writel(value, reg);
	readl(reg);
	udelay(1);
}