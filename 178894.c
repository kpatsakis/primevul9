static void __exit pptp_exit_module(void)
{
	unregister_pppox_proto(PX_PROTO_PPTP);
	proto_unregister(&pptp_sk_proto);
	gre_del_protocol(&gre_pptp_protocol, GREPROTO_PPTP);
	vfree(callid_sock);
}