int ida_get_new(struct ida *ida, int *p_id)
{
	return ida_get_new_above(ida, 0, p_id);
}