static void dccp_mib_exit(void)
{
	free_percpu(dccp_statistics[0]);
	free_percpu(dccp_statistics[1]);
	dccp_statistics[0] = dccp_statistics[1] = NULL;
}