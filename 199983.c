static int __init dccp_mib_init(void)
{
	int rc = -ENOMEM;

	dccp_statistics[0] = alloc_percpu(struct dccp_mib);
	if (dccp_statistics[0] == NULL)
		goto out;

	dccp_statistics[1] = alloc_percpu(struct dccp_mib);
	if (dccp_statistics[1] == NULL)
		goto out_free_one;

	rc = 0;
out:
	return rc;
out_free_one:
	free_percpu(dccp_statistics[0]);
	dccp_statistics[0] = NULL;
	goto out;

}