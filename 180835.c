static int __init setup_fail_page_alloc(char *str)
{
	return setup_fault_attr(&fail_page_alloc.attr, str);
}