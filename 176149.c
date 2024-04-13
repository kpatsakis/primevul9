static OPJ_BOOL opj_j2k_copy_default_tcp_and_create_tcd(opj_j2k_t * p_j2k,
        opj_stream_private_t *p_stream,
        opj_event_mgr_t * p_manager
                                                       )
{
    opj_tcp_t * l_tcp = 00;
    opj_tcp_t * l_default_tcp = 00;
    OPJ_UINT32 l_nb_tiles;
    OPJ_UINT32 i, j;
    opj_tccp_t *l_current_tccp = 00;
    OPJ_UINT32 l_tccp_size;
    OPJ_UINT32 l_mct_size;
    opj_image_t * l_image;
    OPJ_UINT32 l_mcc_records_size, l_mct_records_size;
    opj_mct_data_t * l_src_mct_rec, *l_dest_mct_rec;
    opj_simple_mcc_decorrelation_data_t * l_src_mcc_rec, *l_dest_mcc_rec;
    OPJ_UINT32 l_offset;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_stream);

    l_image = p_j2k->m_private_image;
    l_nb_tiles = p_j2k->m_cp.th * p_j2k->m_cp.tw;
    l_tcp = p_j2k->m_cp.tcps;
    l_tccp_size = l_image->numcomps * (OPJ_UINT32)sizeof(opj_tccp_t);
    l_default_tcp = p_j2k->m_specific_param.m_decoder.m_default_tcp;
    l_mct_size = l_image->numcomps * l_image->numcomps * (OPJ_UINT32)sizeof(
                     OPJ_FLOAT32);

    /* For each tile */
    for (i = 0; i < l_nb_tiles; ++i) {
        /* keep the tile-compo coding parameters pointer of the current tile coding parameters*/
        l_current_tccp = l_tcp->tccps;
        /*Copy default coding parameters into the current tile coding parameters*/
        memcpy(l_tcp, l_default_tcp, sizeof(opj_tcp_t));
        /* Initialize some values of the current tile coding parameters*/
        l_tcp->cod = 0;
        l_tcp->ppt = 0;
        l_tcp->ppt_data = 00;
        l_tcp->m_current_tile_part_number = -1;
        /* Remove memory not owned by this tile in case of early error return. */
        l_tcp->m_mct_decoding_matrix = 00;
        l_tcp->m_nb_max_mct_records = 0;
        l_tcp->m_mct_records = 00;
        l_tcp->m_nb_max_mcc_records = 0;
        l_tcp->m_mcc_records = 00;
        /* Reconnect the tile-compo coding parameters pointer to the current tile coding parameters*/
        l_tcp->tccps = l_current_tccp;

        /* Get the mct_decoding_matrix of the dflt_tile_cp and copy them into the current tile cp*/
        if (l_default_tcp->m_mct_decoding_matrix) {
            l_tcp->m_mct_decoding_matrix = (OPJ_FLOAT32*)opj_malloc(l_mct_size);
            if (! l_tcp->m_mct_decoding_matrix) {
                return OPJ_FALSE;
            }
            memcpy(l_tcp->m_mct_decoding_matrix, l_default_tcp->m_mct_decoding_matrix,
                   l_mct_size);
        }

        /* Get the mct_record of the dflt_tile_cp and copy them into the current tile cp*/
        l_mct_records_size = l_default_tcp->m_nb_max_mct_records * (OPJ_UINT32)sizeof(
                                 opj_mct_data_t);
        l_tcp->m_mct_records = (opj_mct_data_t*)opj_malloc(l_mct_records_size);
        if (! l_tcp->m_mct_records) {
            return OPJ_FALSE;
        }
        memcpy(l_tcp->m_mct_records, l_default_tcp->m_mct_records, l_mct_records_size);

        /* Copy the mct record data from dflt_tile_cp to the current tile*/
        l_src_mct_rec = l_default_tcp->m_mct_records;
        l_dest_mct_rec = l_tcp->m_mct_records;

        for (j = 0; j < l_default_tcp->m_nb_mct_records; ++j) {

            if (l_src_mct_rec->m_data) {

                l_dest_mct_rec->m_data = (OPJ_BYTE*) opj_malloc(l_src_mct_rec->m_data_size);
                if (! l_dest_mct_rec->m_data) {
                    return OPJ_FALSE;
                }
                memcpy(l_dest_mct_rec->m_data, l_src_mct_rec->m_data,
                       l_src_mct_rec->m_data_size);
            }

            ++l_src_mct_rec;
            ++l_dest_mct_rec;
            /* Update with each pass to free exactly what has been allocated on early return. */
            l_tcp->m_nb_max_mct_records += 1;
        }

        /* Get the mcc_record of the dflt_tile_cp and copy them into the current tile cp*/
        l_mcc_records_size = l_default_tcp->m_nb_max_mcc_records * (OPJ_UINT32)sizeof(
                                 opj_simple_mcc_decorrelation_data_t);
        l_tcp->m_mcc_records = (opj_simple_mcc_decorrelation_data_t*) opj_malloc(
                                   l_mcc_records_size);
        if (! l_tcp->m_mcc_records) {
            return OPJ_FALSE;
        }
        memcpy(l_tcp->m_mcc_records, l_default_tcp->m_mcc_records, l_mcc_records_size);
        l_tcp->m_nb_max_mcc_records = l_default_tcp->m_nb_max_mcc_records;

        /* Copy the mcc record data from dflt_tile_cp to the current tile*/
        l_src_mcc_rec = l_default_tcp->m_mcc_records;
        l_dest_mcc_rec = l_tcp->m_mcc_records;

        for (j = 0; j < l_default_tcp->m_nb_max_mcc_records; ++j) {

            if (l_src_mcc_rec->m_decorrelation_array) {
                l_offset = (OPJ_UINT32)(l_src_mcc_rec->m_decorrelation_array -
                                        l_default_tcp->m_mct_records);
                l_dest_mcc_rec->m_decorrelation_array = l_tcp->m_mct_records + l_offset;
            }

            if (l_src_mcc_rec->m_offset_array) {
                l_offset = (OPJ_UINT32)(l_src_mcc_rec->m_offset_array -
                                        l_default_tcp->m_mct_records);
                l_dest_mcc_rec->m_offset_array = l_tcp->m_mct_records + l_offset;
            }

            ++l_src_mcc_rec;
            ++l_dest_mcc_rec;
        }

        /* Copy all the dflt_tile_compo_cp to the current tile cp */
        memcpy(l_current_tccp, l_default_tcp->tccps, l_tccp_size);

        /* Move to next tile cp*/
        ++l_tcp;
    }

    /* Create the current tile decoder*/
    p_j2k->m_tcd = (opj_tcd_t*)opj_tcd_create(OPJ_TRUE); /* FIXME why a cast ? */
    if (! p_j2k->m_tcd) {
        return OPJ_FALSE;
    }

    if (!opj_tcd_init(p_j2k->m_tcd, l_image, &(p_j2k->m_cp), p_j2k->m_tp)) {
        opj_tcd_destroy(p_j2k->m_tcd);
        p_j2k->m_tcd = 00;
        opj_event_msg(p_manager, EVT_ERROR, "Cannot decode tile, memory error\n");
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}