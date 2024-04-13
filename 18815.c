scroll_region_reset(void)
{
    OUT_STR(tgoto((char *)T_CS, (int)Rows - 1, 0));
    if (*T_CSV != NUL)
	OUT_STR(tgoto((char *)T_CSV, (int)Columns - 1, 0));
    screen_start();		    // don't know where cursor is now
}