reset_order_state(void)
{
	memset(&g_order_state, 0, sizeof(g_order_state));
	g_order_state.order_type = RDP_ORDER_PATBLT;
}