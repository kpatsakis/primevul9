static struct clk * __init sunxi_mux_clk_setup(struct device_node *node,
					       const struct mux_data *data,
					       unsigned long flags)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *parents[SUNXI_MAX_PARENTS];
	void __iomem *reg;
	int i;

	reg = of_iomap(node, 0);
	if (!reg) {
		pr_err("Could not map registers for mux-clk: %pOF\n", node);
		return NULL;
	}

	i = of_clk_parent_fill(node, parents, SUNXI_MAX_PARENTS);
	if (of_property_read_string(node, "clock-output-names", &clk_name)) {
		pr_err("%s: could not read clock-output-names from \"%pOF\"\n",
		       __func__, node);
		goto out_unmap;
	}

	clk = clk_register_mux(NULL, clk_name, parents, i,
			       CLK_SET_RATE_PARENT | flags, reg,
			       data->shift, SUNXI_MUX_GATE_WIDTH,
			       0, &clk_lock);

	if (IS_ERR(clk)) {
		pr_err("%s: failed to register mux clock %s: %ld\n", __func__,
		       clk_name, PTR_ERR(clk));
		goto out_unmap;
	}

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		pr_err("%s: failed to add clock provider for %s\n",
		       __func__, clk_name);
		clk_unregister_divider(clk);
		goto out_unmap;
	}

	return clk;
out_unmap:
	iounmap(reg);
	return NULL;
}