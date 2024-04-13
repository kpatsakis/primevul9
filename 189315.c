did_set_title(
    int	    icon)	    /* Did set icon instead of title */
{
    if (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
    {
	maketitle();
	if (icon)
	{
	    if (!p_icon)
		mch_restore_title(2);
	}
	else
	{
	    if (!p_title)
		mch_restore_title(1);
	}
    }
}