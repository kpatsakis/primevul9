win_new_shellsize(void)
{
    static int	old_Rows = 0;
    static int	old_Columns = 0;

    if (old_Rows != Rows || old_Columns != Columns)
	ui_new_shellsize();
    if (old_Rows != Rows)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	if (p_window == old_Rows - 1
		    || (old_Rows == 0 && !option_was_set((char_u *)"window")))
	    p_window = Rows - 1;
	old_Rows = Rows;
	shell_new_rows();	// update window sizes
    }
    if (old_Columns != Columns)
    {
	old_Columns = Columns;
	shell_new_columns();	// update window sizes
    }
}