c_pdf14trans_is_friendly(const gs_composite_t * composite_action, byte cmd0, byte cmd1)
{
    gs_pdf14trans_t *pct0 = (gs_pdf14trans_t *)composite_action;
    int op0 = pct0->params.pdf14_op;

    if (op0 == PDF14_PUSH_DEVICE || op0 == PDF14_END_TRANS_GROUP) {
        /* Halftone commands are always passed to the target printer device,
           because transparency buffers are always contone.
           So we're safe to execute them before queued transparency compositors. */
        if (cmd0 == cmd_opv_extend && (cmd1 == cmd_opv_ext_put_halftone ||
                                       cmd1 == cmd_opv_ext_put_ht_seg))
            return true;
        if (cmd0 == cmd_opv_set_misc && (cmd1 >> 6) == (cmd_set_misc_map >> 6))
            return true;
    }
    return false;
}