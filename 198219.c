static void __init sun4i_apb0_clk_setup(struct device_node *node)
{
	sunxi_divider_clk_setup(node, &sun4i_apb0_data);
}