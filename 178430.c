reg_prev_class(void)
{
    if (rex.input > rex.line)
	return mb_get_class_buf(rex.input - 1
		       - (*mb_head_off)(rex.line, rex.input - 1), rex.reg_buf);
    return -1;
}