static void __init sun6i_pll6_clk_setup(struct device_node *node)
{
	sunxi_divs_clk_setup(node, &sun6i_a31_pll6_divs_data);
}