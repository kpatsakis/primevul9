static int __init pptp_init_module(void)
{
	int err = 0;
	pr_info("PPTP driver version " PPTP_DRIVER_VERSION "\n");

	callid_sock = vzalloc((MAX_CALLID + 1) * sizeof(void *));
	if (!callid_sock)
		return -ENOMEM;

	err = gre_add_protocol(&gre_pptp_protocol, GREPROTO_PPTP);
	if (err) {
		pr_err("PPTP: can't add gre protocol\n");
		goto out_mem_free;
	}

	err = proto_register(&pptp_sk_proto, 0);
	if (err) {
		pr_err("PPTP: can't register sk_proto\n");
		goto out_gre_del_protocol;
	}

	err = register_pppox_proto(PX_PROTO_PPTP, &pppox_pptp_proto);
	if (err) {
		pr_err("PPTP: can't register pppox_proto\n");
		goto out_unregister_sk_proto;
	}

	return 0;

out_unregister_sk_proto:
	proto_unregister(&pptp_sk_proto);
out_gre_del_protocol:
	gre_del_protocol(&gre_pptp_protocol, GREPROTO_PPTP);
out_mem_free:
	vfree(callid_sock);

	return err;
}