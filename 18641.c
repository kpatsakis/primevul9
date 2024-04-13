void __init xfrm_init(void)
{
	register_pernet_subsys(&xfrm_net_ops);
	xfrm_dev_init();
	xfrm_input_init();

#ifdef CONFIG_XFRM_ESPINTCP
	espintcp_init();
#endif
}