static void __init sun4i_cpu_clk_setup(struct device_node *node)
{
	/* Protect CPU clock */
	sunxi_mux_clk_setup(node, &sun4i_cpu_mux_data, CLK_IS_CRITICAL);
}