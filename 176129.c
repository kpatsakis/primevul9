static OPJ_BOOL opj_j2k_merge_ppt(opj_tcp_t *p_tcp, opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i, l_ppt_data_size;
    /* preconditions */
    assert(p_tcp != 00);
    assert(p_manager != 00);
    assert(p_tcp->ppt_buffer == NULL);

    if (p_tcp->ppt == 0U) {
        return OPJ_TRUE;
    }

    l_ppt_data_size = 0U;
    for (i = 0U; i < p_tcp->ppt_markers_count; ++i) {
        l_ppt_data_size +=
            p_tcp->ppt_markers[i].m_data_size; /* can't overflow, max 256 markers of max 65536 bytes */
    }

    p_tcp->ppt_buffer = (OPJ_BYTE *) opj_malloc(l_ppt_data_size);
    if (p_tcp->ppt_buffer == 00) {
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPT marker\n");
        return OPJ_FALSE;
    }
    p_tcp->ppt_len = l_ppt_data_size;
    l_ppt_data_size = 0U;
    for (i = 0U; i < p_tcp->ppt_markers_count; ++i) {
        if (p_tcp->ppt_markers[i].m_data !=
                NULL) { /* standard doesn't seem to require contiguous Zppt */
            memcpy(p_tcp->ppt_buffer + l_ppt_data_size, p_tcp->ppt_markers[i].m_data,
                   p_tcp->ppt_markers[i].m_data_size);
            l_ppt_data_size +=
                p_tcp->ppt_markers[i].m_data_size; /* can't overflow, max 256 markers of max 65536 bytes */

            opj_free(p_tcp->ppt_markers[i].m_data);
            p_tcp->ppt_markers[i].m_data = NULL;
            p_tcp->ppt_markers[i].m_data_size = 0U;
        }
    }

    p_tcp->ppt_markers_count = 0U;
    opj_free(p_tcp->ppt_markers);
    p_tcp->ppt_markers = NULL;

    p_tcp->ppt_data = p_tcp->ppt_buffer;
    p_tcp->ppt_data_size = p_tcp->ppt_len;
    return OPJ_TRUE;
}