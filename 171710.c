static void __exit af_alg_exit(void)
{
	sock_unregister(PF_ALG);
	proto_unregister(&alg_proto);
}