shell_resized_check(void)
{
    int		old_Rows = Rows;
    int		old_Columns = Columns;

    if (!exiting
#ifdef FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    && !gui.starting
#endif
	    )
    {
	(void)ui_get_shellsize();
	check_shellsize();
	if (old_Rows != Rows || old_Columns != Columns)
	    shell_resized();
    }
}