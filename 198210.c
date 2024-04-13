static void __init sun6i_display_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun6i_display_data);
}