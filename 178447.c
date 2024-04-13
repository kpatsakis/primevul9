prog_magic_wrong(void)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    if (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	return FALSE;

    if (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	emsg(_(e_corrupted_regexp_program));
	return TRUE;
    }
    return FALSE;
}