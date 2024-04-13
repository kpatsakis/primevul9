eval_to_string_safe(
    char_u	*arg,
    int		use_sandbox)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    int		save_sc_version = current_sctx.sc_version;
    int		save_garbage = may_garbage_collect;

    current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    if (use_sandbox)
	++sandbox;
    ++textwinlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE);
    if (use_sandbox)
	--sandbox;
    --textwinlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    return retval;
}