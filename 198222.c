static void __init sunxi_divider_clk_setup(struct device_node *node,
					   const struct div_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *clk_parent;
	void __iomem *reg;

	reg = of_iomap(node, 0);
	if (!reg) {
		pr_err("Could not map registers for mux-clk: %pOF\n", node);
		return;
	}

	clk_parent = of_clk_get_parent_name(node, 0);

	if (of_property_read_string(node, "clock-output-names", &clk_name)) {
		pr_err("%s: could not read clock-output-names from \"%pOF\"\n",
		       __func__, node);
		goto out_unmap;
	}

	clk = clk_register_divider_table(NULL, clk_name, clk_parent, 0,
					 reg, data->shift, data->width,
					 data->pow ? CLK_DIVIDER_POWER_OF_TWO : 0,
					 data->table, &clk_lock);
	if (IS_ERR(clk)) {
		pr_err("%s: failed to register divider clock %s: %ld\n",
		       __func__, clk_name, PTR_ERR(clk));
		goto out_unmap;
	}

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		pr_err("%s: failed to add clock provider for %s\n",
		       __func__, clk_name);
		goto out_unregister;
	}

	if (clk_register_clkdev(clk, clk_name, NULL)) {
		of_clk_del_provider(node);
		goto out_unregister;
	}

	return;
out_unregister:
	clk_unregister_divider(clk);

out_unmap:
	iounmap(reg);
}