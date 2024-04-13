static struct clk * __init sunxi_factors_clk_setup(struct device_node *node,
						   const struct factors_data *data)
{
	void __iomem *reg;

	reg = of_iomap(node, 0);
	if (!reg) {
		pr_err("Could not get registers for factors-clk: %pOFn\n",
		       node);
		return NULL;
	}

	return sunxi_factors_register(node, data, &clk_lock, reg);
}