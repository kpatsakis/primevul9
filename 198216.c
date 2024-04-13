static void __init sun4i_pll6_clk_setup(struct device_node *node)
{
	sunxi_divs_clk_setup(node, &pll6_divs_data);
}