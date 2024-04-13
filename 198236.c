static void __init sun5i_ahb_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun5i_a13_ahb_data);
}