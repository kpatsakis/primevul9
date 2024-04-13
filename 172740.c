text_and_win_locked(void)
{
#ifdef FEAT_CMDWIN
    if (cmdwin_type != 0)
	return TRUE;
#endif
    return textwinlock != 0;
}