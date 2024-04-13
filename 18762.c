ins_insert(int replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
		          : replaceState == VREPLACE ? "v"
						     : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    if (State & REPLACE_FLAG)
	State = INSERT | (State & LANGMAP);
    else
	State = replaceState | (State & LANGMAP);
    trigger_modechanged();
    AppendCharToRedobuff(K_INS);
    showmode();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
}