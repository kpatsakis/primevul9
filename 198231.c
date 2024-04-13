static void __init sun4i_pll1_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun4i_pll1_data);
}