static OPJ_BOOL opj_j2k_decode_one_tile(opj_j2k_t *p_j2k,
                                        opj_stream_private_t *p_stream,
                                        opj_event_mgr_t * p_manager)
{
    OPJ_BOOL l_go_on = OPJ_TRUE;
    OPJ_UINT32 l_current_tile_no;
    OPJ_UINT32 l_tile_no_to_dec;
    OPJ_UINT32 l_data_size, l_max_data_size;
    OPJ_INT32 l_tile_x0, l_tile_y0, l_tile_x1, l_tile_y1;
    OPJ_UINT32 l_nb_comps;
    OPJ_BYTE * l_current_data;
    OPJ_UINT32 l_nb_tiles;
    OPJ_UINT32 i;

    l_current_data = (OPJ_BYTE*)opj_malloc(1000);
    if (! l_current_data) {
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to decode one tile\n");
        return OPJ_FALSE;
    }
    l_max_data_size = 1000;

    /*Allocate and initialize some elements of codestrem index if not already done*/
    if (!p_j2k->cstr_index->tile_index) {
        if (!opj_j2k_allocate_tile_element_cstr_index(p_j2k)) {
            opj_free(l_current_data);
            return OPJ_FALSE;
        }
    }
    /* Move into the codestream to the first SOT used to decode the desired tile */
    l_tile_no_to_dec = (OPJ_UINT32)
                       p_j2k->m_specific_param.m_decoder.m_tile_ind_to_dec;
    if (p_j2k->cstr_index->tile_index)
        if (p_j2k->cstr_index->tile_index->tp_index) {
            if (! p_j2k->cstr_index->tile_index[l_tile_no_to_dec].nb_tps) {
                /* the index for this tile has not been built,
                 *  so move to the last SOT read */
                if (!(opj_stream_read_seek(p_stream,
                                           p_j2k->m_specific_param.m_decoder.m_last_sot_read_pos + 2, p_manager))) {
                    opj_event_msg(p_manager, EVT_ERROR, "Problem with seek function\n");
                    opj_free(l_current_data);
                    return OPJ_FALSE;
                }
            } else {
                if (!(opj_stream_read_seek(p_stream,
                                           p_j2k->cstr_index->tile_index[l_tile_no_to_dec].tp_index[0].start_pos + 2,
                                           p_manager))) {
                    opj_event_msg(p_manager, EVT_ERROR, "Problem with seek function\n");
                    opj_free(l_current_data);
                    return OPJ_FALSE;
                }
            }
            /* Special case if we have previously read the EOC marker (if the previous tile getted is the last ) */
            if (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_EOC) {
                p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_TPHSOT;
            }
        }

    /* Reset current tile part number for all tiles, and not only the one */
    /* of interest. */
    /* Not completely sure this is always correct but required for */
    /* ./build/bin/j2k_random_tile_access ./build/tests/tte1.j2k */
    l_nb_tiles = p_j2k->m_cp.tw * p_j2k->m_cp.th;
    for (i = 0; i < l_nb_tiles; ++i) {
        p_j2k->m_cp.tcps[i].m_current_tile_part_number = -1;
    }

    for (;;) {
        if (! opj_j2k_read_tile_header(p_j2k,
                                       &l_current_tile_no,
                                       &l_data_size,
                                       &l_tile_x0, &l_tile_y0,
                                       &l_tile_x1, &l_tile_y1,
                                       &l_nb_comps,
                                       &l_go_on,
                                       p_stream,
                                       p_manager)) {
            opj_free(l_current_data);
            return OPJ_FALSE;
        }

        if (! l_go_on) {
            break;
        }

        if (l_data_size > l_max_data_size) {
            OPJ_BYTE *l_new_current_data = (OPJ_BYTE *) opj_realloc(l_current_data,
                                           l_data_size);
            if (! l_new_current_data) {
                opj_free(l_current_data);
                l_current_data = NULL;
                opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to decode tile %d/%d\n",
                              l_current_tile_no + 1, p_j2k->m_cp.th * p_j2k->m_cp.tw);
                return OPJ_FALSE;
            }
            l_current_data = l_new_current_data;
            l_max_data_size = l_data_size;
        }

        if (! opj_j2k_decode_tile(p_j2k, l_current_tile_no, l_current_data, l_data_size,
                                  p_stream, p_manager)) {
            opj_free(l_current_data);
            return OPJ_FALSE;
        }
        opj_event_msg(p_manager, EVT_INFO, "Tile %d/%d has been decoded.\n",
                      l_current_tile_no + 1, p_j2k->m_cp.th * p_j2k->m_cp.tw);

        if (! opj_j2k_update_image_data(p_j2k->m_tcd, l_current_data,
                                        p_j2k->m_output_image)) {
            opj_free(l_current_data);
            return OPJ_FALSE;
        }
        opj_event_msg(p_manager, EVT_INFO,
                      "Image data has been updated with tile %d.\n\n", l_current_tile_no + 1);

        if (l_current_tile_no == l_tile_no_to_dec) {
            /* move into the codestream to the first SOT (FIXME or not move?)*/
            if (!(opj_stream_read_seek(p_stream, p_j2k->cstr_index->main_head_end + 2,
                                       p_manager))) {
                opj_event_msg(p_manager, EVT_ERROR, "Problem with seek function\n");
                opj_free(l_current_data);
                return OPJ_FALSE;
            }
            break;
        } else {
            opj_event_msg(p_manager, EVT_WARNING,
                          "Tile read, decoded and updated is not the desired one (%d vs %d).\n",
                          l_current_tile_no + 1, l_tile_no_to_dec + 1);
        }

    }

    opj_free(l_current_data);

    return OPJ_TRUE;
}