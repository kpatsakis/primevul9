static int __init ipc_ns_init(void)
{
	shm_init_ns(&init_ipc_ns);
	return 0;
}