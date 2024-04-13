static int __init af_alg_init(void)
{
	int err = proto_register(&alg_proto, 0);

	if (err)
		goto out;

	err = sock_register(&alg_family);
	if (err != 0)
		goto out_unregister_proto;

out:
	return err;

out_unregister_proto:
	proto_unregister(&alg_proto);
	goto out;
}