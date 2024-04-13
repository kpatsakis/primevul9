f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_number = p != NULL ? p->cmdspos + 1 : 0;
}