get_termcode(int i)
{
    if (i >= tc_len)
	return NULL;
    return &termcodes[i].name[0];
}