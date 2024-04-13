find_opening_op(int opening_op, gs_composite_t **ppcte,
                gs_compositor_closing_state return_code)
{
    /* Assuming a right *BEGIN* - *END* operation balance. */
    gs_composite_t *pcte = *ppcte;

    for (;;) {
        if (pcte->type->comp_id == GX_COMPOSITOR_PDF14_TRANS) {
            gs_pdf14trans_t *pct = (gs_pdf14trans_t *)pcte;
            int op = pct->params.pdf14_op;

            *ppcte = pcte;
            if (op == opening_op)
                return return_code;
            if (op != PDF14_SET_BLEND_PARAMS) {
                if (opening_op == PDF14_BEGIN_TRANS_MASK)
                    return COMP_ENQUEUE;
                if (opening_op == PDF14_BEGIN_TRANS_GROUP) {
                    if (op != PDF14_BEGIN_TRANS_MASK && op != PDF14_END_TRANS_MASK)
                        return COMP_ENQUEUE;
                }
                if (opening_op == PDF14_PUSH_DEVICE) {
                    if (op != PDF14_BEGIN_TRANS_MASK && op != PDF14_END_TRANS_MASK &&
                        op != PDF14_BEGIN_TRANS_GROUP && op != PDF14_END_TRANS_GROUP)
                        return COMP_ENQUEUE;
                }
            }
        } else
            return COMP_ENQUEUE;
        pcte = pcte->prev;
        if (pcte == NULL)
            return COMP_EXEC_QUEUE; /* Not in queue. */
    }
}