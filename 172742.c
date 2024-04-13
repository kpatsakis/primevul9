get_ccline_ptr(void)
{
    if ((State & CMDLINE) == 0)
	return NULL;
    if (ccline.cmdbuff != NULL)
	return &ccline;
    if (prev_ccline_used && prev_ccline.cmdbuff != NULL)
	return &prev_ccline;
    return NULL;
}