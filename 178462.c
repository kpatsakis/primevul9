free_regexp_stuff(void)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
}