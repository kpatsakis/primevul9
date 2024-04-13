static void __init sun7i_out_clk_setup(struct device_node *node)
{
	sunxi_factors_clk_setup(node, &sun7i_a20_out_data);
}