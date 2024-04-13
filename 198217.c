static void __init sun6i_ahb1_mux_clk_setup(struct device_node *node)
{
	sunxi_mux_clk_setup(node, &sun6i_a31_ahb1_mux_data, 0);
}