static void __init sun4i_pll5_clk_setup(struct device_node *node)
{
	sunxi_divs_clk_setup(node, &pll5_divs_data);
}