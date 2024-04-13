static OPJ_BOOL opj_j2k_read_soc(opj_j2k_t *p_j2k,
                                 opj_stream_private_t *p_stream,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_BYTE l_data [2];
    OPJ_UINT32 l_marker;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    if (opj_stream_read_data(p_stream, l_data, 2, p_manager) != 2) {
        return OPJ_FALSE;
    }

    opj_read_bytes(l_data, &l_marker, 2);
    if (l_marker != J2K_MS_SOC) {
        return OPJ_FALSE;
    }

    /* Next marker should be a SIZ marker in the main header */
    p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_MHSIZ;

    /* FIXME move it in a index structure included in p_j2k*/
    p_j2k->cstr_index->main_head_start = opj_stream_tell(p_stream) - 2;

    opj_event_msg(p_manager, EVT_INFO, "Start to read j2k main header (%d).\n",
                  p_j2k->cstr_index->main_head_start);

    /* Add the marker to the codestream index*/
    if (OPJ_FALSE == opj_j2k_add_mhmarker(p_j2k->cstr_index, J2K_MS_SOC,
                                          p_j2k->cstr_index->main_head_start, 2)) {
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to add mh marker\n");
        return OPJ_FALSE;
    }
    return OPJ_TRUE;
}