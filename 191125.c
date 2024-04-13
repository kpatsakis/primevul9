gs_finit_push_systemdict(i_ctx_t *i_ctx_p)
{
    if (i_ctx_p == NULL)
        return;
    if (dsp == dstop ) {
        if (ref_stack_extend(&d_stack, 1) < 0) {
            /* zend() cannot fail */
            (void)zend(i_ctx_p);
        }
    }
    dsp++;
    ref_assign(dsp, systemdict);
}