static OPJ_BOOL opj_j2k_read_poc(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 i, l_nb_comp, l_tmp;
    opj_image_t * l_image = 00;
    OPJ_UINT32 l_old_poc_nb, l_current_poc_nb, l_current_poc_remaining;
    OPJ_UINT32 l_chunk_size, l_comp_room;

    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    opj_poc_t *l_current_poc = 00;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_image = p_j2k->m_private_image;
    l_nb_comp = l_image->numcomps;
    if (l_nb_comp <= 256) {
        l_comp_room = 1;
    } else {
        l_comp_room = 2;
    }
    l_chunk_size = 5 + 2 * l_comp_room;
    l_current_poc_nb = p_header_size / l_chunk_size;
    l_current_poc_remaining = p_header_size % l_chunk_size;

    if ((l_current_poc_nb <= 0) || (l_current_poc_remaining != 0)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading POC marker\n");
        return OPJ_FALSE;
    }

    l_cp = &(p_j2k->m_cp);
    l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH) ?
            &l_cp->tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;
    l_old_poc_nb = l_tcp->POC ? l_tcp->numpocs + 1 : 0;
    l_current_poc_nb += l_old_poc_nb;

    if (l_current_poc_nb >= 32) {
        opj_event_msg(p_manager, EVT_ERROR, "Too many POCs %d\n", l_current_poc_nb);
        return OPJ_FALSE;
    }
    assert(l_current_poc_nb < 32);

    /* now poc is in use.*/
    l_tcp->POC = 1;

    l_current_poc = &l_tcp->pocs[l_old_poc_nb];
    for (i = l_old_poc_nb; i < l_current_poc_nb; ++i) {
        opj_read_bytes(p_header_data, &(l_current_poc->resno0),
                       1);                               /* RSpoc_i */
        ++p_header_data;
        opj_read_bytes(p_header_data, &(l_current_poc->compno0),
                       l_comp_room);  /* CSpoc_i */
        p_header_data += l_comp_room;
        opj_read_bytes(p_header_data, &(l_current_poc->layno1),
                       2);                               /* LYEpoc_i */
        /* make sure layer end is in acceptable bounds */
        l_current_poc->layno1 = opj_uint_min(l_current_poc->layno1, l_tcp->numlayers);
        p_header_data += 2;
        opj_read_bytes(p_header_data, &(l_current_poc->resno1),
                       1);                               /* REpoc_i */
        ++p_header_data;
        opj_read_bytes(p_header_data, &(l_current_poc->compno1),
                       l_comp_room);  /* CEpoc_i */
        p_header_data += l_comp_room;
        opj_read_bytes(p_header_data, &l_tmp,
                       1);                                                                 /* Ppoc_i */
        ++p_header_data;
        l_current_poc->prg = (OPJ_PROG_ORDER) l_tmp;
        /* make sure comp is in acceptable bounds */
        l_current_poc->compno1 = opj_uint_min(l_current_poc->compno1, l_nb_comp);
        ++l_current_poc;
    }

    l_tcp->numpocs = l_current_poc_nb - 1;
    return OPJ_TRUE;
}