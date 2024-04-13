c_pdf14trans_read(gs_composite_t * * ppct, const byte *	data,
                                uint size, gs_memory_t * mem )
{
    gs_pdf14trans_params_t params = {0};
    const byte * start = data;
    int used, code = 0;

    if (size < 1)
        return_error(gs_error_rangecheck);

    /* Read PDF 1.4 compositor data from the clist */
    params.pdf14_op = *data++;
    if_debug2m('v', mem, "[v] c_pdf14trans_read: opcode = %s  avail = %d",
               pdf14_opcode_names[params.pdf14_op], size);
    memset(&params.ctm, 0, sizeof(params.ctm));
    switch (params.pdf14_op) {
        default:			/* Should not occur. */
            break;
        case PDF14_PUSH_DEVICE:
            read_value(data, params.num_spot_colors);
            read_value(data, params.is_pattern);
            break;
        case PDF14_ABORT_DEVICE:
            break;
        case PDF14_POP_DEVICE:
            read_value(data, params.is_pattern);
            break;
        case PDF14_END_TRANS_GROUP:
            code += 0; /* A good place for a breakpoint. */
            break;			/* No data */
        case PDF14_PUSH_TRANS_STATE:
            break;
        case PDF14_POP_TRANS_STATE:
            break;
        case PDF14_BEGIN_TRANS_GROUP:
            /*
             * We are currently not using the bbox or the colorspace so they were
             * not placed in the clist
             */
            data = cmd_read_matrix(&params.ctm, data);
            params.Isolated = (*data) & 1;
            params.Knockout = (*data++ >> 1) & 1;
            params.blend_mode = *data++;
            params.group_color = *data++;  /* Trans group color */
            read_value(data,params.group_color_numcomps);  /* color group size */
            read_value(data, params.opacity.alpha);
            read_value(data, params.shape.alpha);
            read_value(data, params.bbox);
            read_value(data, params.mask_id);
            read_value(data, params.icc_hash);
            break;
        case PDF14_BEGIN_TRANS_MASK:
                /* This is the largest transparency parameter at this time (potentially
                 * 1531 bytes in size if Background_components =
                 * GS_CLIENT_COLOR_MAX_COMPONENTS and Matte_components =
                 * GS_CLIENT_COLOR_MAX_COMPONENTS and we have a transfer function as well).
                 *
                 * NOTE:
                 * The clist reader must be able to handle this sized device.
                 * If any changes are made here the #define MAX_CLIST_COMPOSITOR_SIZE
                 * may also need to be changed correspondingly (defined in gstparam.h)
                 * Also... if another compositor param should exceed this size, this
                 * same condition applies.
                 */
            data = cmd_read_matrix(&params.ctm, data);
            read_value(data, params.subtype);
            params.group_color = *data++;
            read_value(data, params.group_color_numcomps);
            params.replacing = *data++;
            params.function_is_identity = *data++;
            params.Background_components = *data++;
            params.Matte_components = *data++;
            read_value(data, params.bbox);
            read_value(data, params.mask_id);
            if (params.Background_components) {
                const int l = sizeof(params.Background[0]) * params.Background_components;

                memcpy(params.Background, data, l);
                data += l;
                memcpy(&params.GrayBackground, data, sizeof(params.GrayBackground));
                data += sizeof(params.GrayBackground);
            }
            if (params.Matte_components) {
                const int m = sizeof(params.Matte[0]) * params.Matte_components;

                memcpy(params.Matte, data, m);
                data += m;
            }
            read_value(data, params.icc_hash);
            if (params.function_is_identity) {
                int i;

                for (i = 0; i < MASK_TRANSFER_FUNCTION_SIZE; i++) {
                    params.transfer_fn[i] = (byte)floor(i *
                        (255.0 / (MASK_TRANSFER_FUNCTION_SIZE - 1)) + 0.5);
                }
            } else {
                read_value(data, params.transfer_fn);
            }
            break;
        case PDF14_END_TRANS_MASK:
            break;
        case PDF14_PUSH_SMASK_COLOR:
            return 0;
            break;
        case PDF14_POP_SMASK_COLOR:
            return 0;
            break;
        case PDF14_SET_BLEND_PARAMS:
            params.changed = *data++;
            if (params.changed & PDF14_SET_BLEND_MODE)
                params.blend_mode = *data++;
            if (params.changed & PDF14_SET_TEXT_KNOCKOUT)
                params.text_knockout = *data++;
            if (params.changed & PDF14_SET_OPACITY_ALPHA)
                read_value(data, params.opacity.alpha);
            if (params.changed & PDF14_SET_SHAPE_ALPHA)
                read_value(data, params.shape.alpha);
            if (params.changed & PDF14_SET_OVERPRINT)
                read_value(data, params.overprint);
            if (params.changed & PDF14_SET_OVERPRINT_MODE)
                read_value(data, params.overprint_mode);
            break;
    }
    code = gs_create_pdf14trans(ppct, &params, mem);
    if (code < 0)
        return code;
    used = data - start;
    if_debug2m('v', mem, " mask_id=%d used = %d\n", params.mask_id, used);

    /* If we read more than the maximum expected, return a rangecheck error */
    if ( used + 3 > MAX_CLIST_COMPOSITOR_SIZE )
        return_error(gs_error_rangecheck);
    else
        return used;
}