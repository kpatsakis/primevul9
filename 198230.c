static void __init sun6i_pll1_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun6i_a31_pll1_data);
}