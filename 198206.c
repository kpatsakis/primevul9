static void __init sun4i_axi_clk_setup(struct device_node *node)
{
	sunxi_divider_clk_setup(node, &sun4i_axi_data);
}