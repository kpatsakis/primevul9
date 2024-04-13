term_push_title(int which)
{
    if ((which & SAVE_RESTORE_TITLE) && T_CST != NULL && *T_CST != NUL)
    {
	OUT_STR(T_CST);
	out_flush();
    }

    if ((which & SAVE_RESTORE_ICON) && T_SSI != NULL && *T_SSI != NUL)
    {
	OUT_STR(T_SSI);
	out_flush();
    }
}