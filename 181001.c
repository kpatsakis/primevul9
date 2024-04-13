store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    int		    flags = ASSIGN_DECL;

    if (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, NULL, tv, FALSE, flags, 0);
    restore_funccal();
}