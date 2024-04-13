static int __init setup_fail_make_request(char *str)
{
	return setup_fault_attr(&fail_make_request, str);
}