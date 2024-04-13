static void __init sun8i_ahb2_clk_setup(struct device_node *node)
{
	sunxi_mux_clk_setup(node, &sun8i_h3_ahb2_mux_data, 0);
}