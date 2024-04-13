OPJ_BOOL opj_j2k_encode(opj_j2k_t * p_j2k,
                        opj_stream_private_t *p_stream,
                        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i, j;
    OPJ_UINT32 l_nb_tiles;
    OPJ_UINT32 l_max_tile_size = 0, l_current_tile_size;
    OPJ_BYTE * l_current_data = 00;
    OPJ_BOOL l_reuse_data = OPJ_FALSE;
    opj_tcd_t* p_tcd = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    p_tcd = p_j2k->m_tcd;

    l_nb_tiles = p_j2k->m_cp.th * p_j2k->m_cp.tw;
    if (l_nb_tiles == 1) {
        l_reuse_data = OPJ_TRUE;
#ifdef __SSE__
        for (j = 0; j < p_j2k->m_tcd->image->numcomps; ++j) {
            opj_image_comp_t * l_img_comp = p_tcd->image->comps + j;
            if (((size_t)l_img_comp->data & 0xFU) !=
                    0U) { /* tile data shall be aligned on 16 bytes */
                l_reuse_data = OPJ_FALSE;
            }
        }
#endif
    }
    for (i = 0; i < l_nb_tiles; ++i) {
        if (! opj_j2k_pre_write_tile(p_j2k, i, p_stream, p_manager)) {
            if (l_current_data) {
                opj_free(l_current_data);
            }
            return OPJ_FALSE;
        }

        /* if we only have one tile, then simply set tile component data equal to image component data */
        /* otherwise, allocate the data */
        for (j = 0; j < p_j2k->m_tcd->image->numcomps; ++j) {
            opj_tcd_tilecomp_t* l_tilec = p_tcd->tcd_image->tiles->comps + j;
            if (l_reuse_data) {
                opj_image_comp_t * l_img_comp = p_tcd->image->comps + j;
                l_tilec->data  =  l_img_comp->data;
                l_tilec->ownsData = OPJ_FALSE;
            } else {
                if (! opj_alloc_tile_component_data(l_tilec)) {
                    opj_event_msg(p_manager, EVT_ERROR, "Error allocating tile component data.");
                    if (l_current_data) {
                        opj_free(l_current_data);
                    }
                    return OPJ_FALSE;
                }
            }
        }
        l_current_tile_size = opj_tcd_get_encoded_tile_size(p_j2k->m_tcd);
        if (!l_reuse_data) {
            if (l_current_tile_size > l_max_tile_size) {
                OPJ_BYTE *l_new_current_data = (OPJ_BYTE *) opj_realloc(l_current_data,
                                               l_current_tile_size);
                if (! l_new_current_data) {
                    if (l_current_data) {
                        opj_free(l_current_data);
                    }
                    opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to encode all tiles\n");
                    return OPJ_FALSE;
                }
                l_current_data = l_new_current_data;
                l_max_tile_size = l_current_tile_size;
            }

            /* copy image data (32 bit) to l_current_data as contiguous, all-component, zero offset buffer */
            /* 32 bit components @ 8 bit precision get converted to 8 bit */
            /* 32 bit components @ 16 bit precision get converted to 16 bit */
            opj_j2k_get_tile_data(p_j2k->m_tcd, l_current_data);

            /* now copy this data into the tile component */
            if (! opj_tcd_copy_tile_data(p_j2k->m_tcd, l_current_data,
                                         l_current_tile_size)) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Size mismatch between tile data and sent data.");
                opj_free(l_current_data);
                return OPJ_FALSE;
            }
        }

        if (! opj_j2k_post_write_tile(p_j2k, p_stream, p_manager)) {
            if (l_current_data) {
                opj_free(l_current_data);
            }
            return OPJ_FALSE;
        }
    }

    if (l_current_data) {
        opj_free(l_current_data);
    }
    return OPJ_TRUE;
}