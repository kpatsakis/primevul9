input_get(struct input_ctx *ictx, u_int validx, int minval, int defval)
{
	int	retval;

	if (validx >= ictx->param_list_len)
	    return (defval);

	retval = ictx->param_list[validx];
	if (retval == -1)
		return (defval);
	if (retval < minval)
		return (minval);
	return (retval);
}