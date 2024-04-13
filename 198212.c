static void __init sun8i_axi_clk_setup(struct device_node *node)
{
	sunxi_divider_clk_setup(node, &sun8i_a23_axi_data);
}