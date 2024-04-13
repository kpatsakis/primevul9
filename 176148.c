static OPJ_BOOL opj_j2k_decode_tiles(opj_j2k_t *p_j2k,
                                     opj_stream_private_t *p_stream,
                                     opj_event_mgr_t * p_manager)
{
    OPJ_BOOL l_go_on = OPJ_TRUE;
    OPJ_UINT32 l_current_tile_no;
    OPJ_UINT32 l_data_size, l_max_data_size;
    OPJ_INT32 l_tile_x0, l_tile_y0, l_tile_x1, l_tile_y1;
    OPJ_UINT32 l_nb_comps;
    OPJ_BYTE * l_current_data;
    OPJ_UINT32 nr_tiles = 0;

    /* Particular case for whole single tile decoding */
    /* We can avoid allocating intermediate tile buffers */
    if (p_j2k->m_cp.tw == 1 && p_j2k->m_cp.th == 1 &&
            p_j2k->m_cp.tx0 == 0 && p_j2k->m_cp.ty0 == 0 &&
            p_j2k->m_output_image->x0 == 0 &&
            p_j2k->m_output_image->y0 == 0 &&
            p_j2k->m_output_image->x1 == p_j2k->m_cp.tdx &&
            p_j2k->m_output_image->y1 == p_j2k->m_cp.tdy &&
            p_j2k->m_output_image->comps[0].factor == 0) {
        OPJ_UINT32 i;
        if (! opj_j2k_read_tile_header(p_j2k,
                                       &l_current_tile_no,
                                       &l_data_size,
                                       &l_tile_x0, &l_tile_y0,
                                       &l_tile_x1, &l_tile_y1,
                                       &l_nb_comps,
                                       &l_go_on,
                                       p_stream,
                                       p_manager)) {
            return OPJ_FALSE;
        }

        if (! opj_j2k_decode_tile(p_j2k, l_current_tile_no, NULL, 0,
                                  p_stream, p_manager)) {
            opj_event_msg(p_manager, EVT_ERROR, "Failed to decode tile 1/1\n");
            return OPJ_FALSE;
        }

        /* Transfer TCD data to output image data */
        for (i = 0; i < p_j2k->m_output_image->numcomps; i++) {
            opj_image_data_free(p_j2k->m_output_image->comps[i].data);
            p_j2k->m_output_image->comps[i].data =
                p_j2k->m_tcd->tcd_image->tiles->comps[i].data;
            p_j2k->m_output_image->comps[i].resno_decoded =
                p_j2k->m_tcd->image->comps[i].resno_decoded;
            p_j2k->m_tcd->tcd_image->tiles->comps[i].data = NULL;
        }

        return OPJ_TRUE;
    }

    l_current_data = (OPJ_BYTE*)opj_malloc(1000);
    if (! l_current_data) {
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to decode tiles\n");
        return OPJ_FALSE;
    }
    l_max_data_size = 1000;

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
            opj_event_msg(p_manager, EVT_ERROR, "Failed to decode tile %d/%d\n",
                          l_current_tile_no + 1, p_j2k->m_cp.th * p_j2k->m_cp.tw);
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

        if (opj_stream_get_number_byte_left(p_stream) == 0
                && p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_NEOC) {
            break;
        }
        if (++nr_tiles ==  p_j2k->m_cp.th * p_j2k->m_cp.tw) {
            break;
        }
    }

    opj_free(l_current_data);

    return OPJ_TRUE;
}