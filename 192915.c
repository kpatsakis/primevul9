ex_pwd(exarg_T *eap UNUSED)
{
    if (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	if (p_verbose > 0)
	{
	    char *context = "global";

	    if (last_chdir_reason != NULL)
		context = last_chdir_reason;
	    else if (curwin->w_localdir != NULL)
		context = "window";
	    else if (curtab->tp_localdir != NULL)
		context = "tabpage";
	    smsg("[%s] %s", context, (char *)NameBuff);
	}
	else
	    msg((char *)NameBuff);
    }
    else
	emsg(_("E187: Unknown"));
}