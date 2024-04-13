static OPJ_BOOL opj_j2k_write_sod(opj_j2k_t *p_j2k,
                                  opj_tcd_t * p_tile_coder,
                                  OPJ_BYTE * p_data,
                                  OPJ_UINT32 * p_data_written,
                                  OPJ_UINT32 p_total_data_size,
                                  const opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager
                                 )
{
    opj_codestream_info_t *l_cstr_info = 00;
    OPJ_UINT32 l_remaining_data;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    OPJ_UNUSED(p_stream);

    if (p_total_data_size < 4) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough bytes in output buffer to write SOD marker\n");
        return OPJ_FALSE;
    }

    opj_write_bytes(p_data, J2K_MS_SOD,
                    2);                                 /* SOD */
    p_data += 2;

    /* make room for the EOF marker */
    l_remaining_data =  p_total_data_size - 4;

    /* update tile coder */
    p_tile_coder->tp_num =
        p_j2k->m_specific_param.m_encoder.m_current_poc_tile_part_number ;
    p_tile_coder->cur_tp_num =
        p_j2k->m_specific_param.m_encoder.m_current_tile_part_number;

    /* INDEX >> */
    /* TODO mergeV2: check this part which use cstr_info */
    /*l_cstr_info = p_j2k->cstr_info;
    if (l_cstr_info) {
            if (!p_j2k->m_specific_param.m_encoder.m_current_tile_part_number ) {
                    //TODO cstr_info->tile[p_j2k->m_current_tile_number].end_header = p_stream_tell(p_stream) + p_j2k->pos_correction - 1;
                    l_cstr_info->tile[p_j2k->m_current_tile_number].tileno = p_j2k->m_current_tile_number;
            }
            else {*/
    /*
    TODO
    if
            (cstr_info->tile[p_j2k->m_current_tile_number].packet[cstr_info->packno - 1].end_pos < p_stream_tell(p_stream))
    {
            cstr_info->tile[p_j2k->m_current_tile_number].packet[cstr_info->packno].start_pos = p_stream_tell(p_stream);
    }*/
    /*}*/
    /* UniPG>> */
#ifdef USE_JPWL
    /* update markers struct */
    /*OPJ_BOOL res = j2k_add_marker(p_j2k->cstr_info, J2K_MS_SOD, p_j2k->sod_start, 2);
    */
    assert(0 && "TODO");
#endif /* USE_JPWL */
    /* <<UniPG */
    /*}*/
    /* << INDEX */

    if (p_j2k->m_specific_param.m_encoder.m_current_tile_part_number == 0) {
        p_tile_coder->tcd_image->tiles->packno = 0;
        if (l_cstr_info) {
            l_cstr_info->packno = 0;
        }
    }

    *p_data_written = 0;

    if (! opj_tcd_encode_tile(p_tile_coder, p_j2k->m_current_tile_number, p_data,
                              p_data_written, l_remaining_data, l_cstr_info,
                              p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Cannot encode tile\n");
        return OPJ_FALSE;
    }

    *p_data_written += 2;

    return OPJ_TRUE;
}