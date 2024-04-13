static void __init sun8i_pll1_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun8i_a23_pll1_data);
}