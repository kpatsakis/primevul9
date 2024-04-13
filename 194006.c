queue_var_show(unsigned long var, char *page)
{
	return sprintf(page, "%lu\n", var);
}