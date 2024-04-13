static void __init sun4i_apb1_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun4i_apb1_data);
}