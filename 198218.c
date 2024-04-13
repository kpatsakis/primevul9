static void __init sun4i_ahb_clk_setup(struct device_node *node)
{
	sunxi_divider_clk_setup(node, &sun4i_ahb_data);
}