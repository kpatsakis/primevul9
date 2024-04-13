nv_mouse(cmdarg_T *cap)
{
    (void)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}