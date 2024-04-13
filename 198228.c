static void __init sun6i_ahb1_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun6i_ahb1_data);
}