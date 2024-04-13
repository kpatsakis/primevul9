c_pdf14trans_is_closing(const gs_composite_t * composite_action, gs_composite_t ** ppcte,
                        gx_device *dev)
{
    gs_pdf14trans_t *pct0 = (gs_pdf14trans_t *)composite_action;
    int op0 = pct0->params.pdf14_op;

    switch (op0) {
        default: return_error(gs_error_unregistered); /* Must not happen. */
        case PDF14_PUSH_DEVICE:
            return COMP_ENQUEUE;
        case PDF14_ABORT_DEVICE:
            return COMP_ENQUEUE;
        case PDF14_POP_DEVICE:
            if (*ppcte == NULL)
                return COMP_ENQUEUE;
            else {
                gs_compositor_closing_state state = find_opening_op(PDF14_PUSH_DEVICE, ppcte, COMP_EXEC_IDLE);

                if (state == COMP_EXEC_IDLE)
                    return COMP_DROP_QUEUE;
                return state;
            }
        case PDF14_BEGIN_TRANS_GROUP:
            return COMP_ENQUEUE;
        case PDF14_END_TRANS_GROUP:
            if (*ppcte == NULL)
                return COMP_EXEC_QUEUE;
            return find_opening_op(PDF14_BEGIN_TRANS_GROUP, ppcte, COMP_MARK_IDLE);
        case PDF14_BEGIN_TRANS_MASK:
            return COMP_ENQUEUE;
        case PDF14_PUSH_TRANS_STATE:
            return COMP_ENQUEUE;
        case PDF14_POP_TRANS_STATE:
            return COMP_ENQUEUE;
        case PDF14_PUSH_SMASK_COLOR:
            return COMP_ENQUEUE;
            break;
        case PDF14_POP_SMASK_COLOR:
            return COMP_ENQUEUE;
            break;
        case PDF14_END_TRANS_MASK:
            if (*ppcte == NULL)
                return COMP_EXEC_QUEUE;
            return find_opening_op(PDF14_BEGIN_TRANS_MASK, ppcte, COMP_MARK_IDLE);
        case PDF14_SET_BLEND_PARAMS:
            if (*ppcte == NULL)
                return COMP_ENQUEUE;
            /* hack : ignore csel - here it is always zero : */
            return find_same_op(composite_action, PDF14_SET_BLEND_PARAMS, ppcte);
    }
}