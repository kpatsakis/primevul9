static void __init sun7i_pll4_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun7i_a20_pll4_data);
}