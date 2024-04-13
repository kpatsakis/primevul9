static OPJ_BOOL opj_j2k_read_cod(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    /* loop */
    OPJ_UINT32 i;
    OPJ_UINT32 l_tmp;
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    opj_image_t *l_image = 00;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_image = p_j2k->m_private_image;
    l_cp = &(p_j2k->m_cp);

    /* If we are in the first tile-part header of the current tile */
    l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH) ?
            &l_cp->tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    /* Only one COD per tile */
    if (l_tcp->cod) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "COD marker already read. No more than one COD marker per tile.\n");
        return OPJ_FALSE;
    }
    l_tcp->cod = 1;

    /* Make sure room is sufficient */
    if (p_header_size < 5) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading COD marker\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_tcp->csty, 1);         /* Scod */
    ++p_header_data;
    /* Make sure we know how to decode this */
    if ((l_tcp->csty & ~(OPJ_UINT32)(J2K_CP_CSTY_PRT | J2K_CP_CSTY_SOP |
                                     J2K_CP_CSTY_EPH)) != 0U) {
        opj_event_msg(p_manager, EVT_ERROR, "Unknown Scod value in COD marker\n");
        return OPJ_FALSE;
    }
    opj_read_bytes(p_header_data, &l_tmp, 1);                       /* SGcod (A) */
    ++p_header_data;
    l_tcp->prg = (OPJ_PROG_ORDER) l_tmp;
    /* Make sure progression order is valid */
    if (l_tcp->prg > OPJ_CPRL) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Unknown progression order in COD marker\n");
        l_tcp->prg = OPJ_PROG_UNKNOWN;
    }
    opj_read_bytes(p_header_data, &l_tcp->numlayers, 2);    /* SGcod (B) */
    p_header_data += 2;

    if ((l_tcp->numlayers < 1U) || (l_tcp->numlayers > 65535U)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Invalid number of layers in COD marker : %d not in range [1-65535]\n",
                      l_tcp->numlayers);
        return OPJ_FALSE;
    }

    /* If user didn't set a number layer to decode take the max specify in the codestream. */
    if (l_cp->m_specific_param.m_dec.m_layer) {
        l_tcp->num_layers_to_decode = l_cp->m_specific_param.m_dec.m_layer;
    } else {
        l_tcp->num_layers_to_decode = l_tcp->numlayers;
    }

    opj_read_bytes(p_header_data, &l_tcp->mct, 1);          /* SGcod (C) */
    ++p_header_data;

    p_header_size -= 5;
    for (i = 0; i < l_image->numcomps; ++i) {
        l_tcp->tccps[i].csty = l_tcp->csty & J2K_CCP_CSTY_PRT;
    }

    if (! opj_j2k_read_SPCod_SPCoc(p_j2k, 0, p_header_data, &p_header_size,
                                   p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading COD marker\n");
        return OPJ_FALSE;
    }

    if (p_header_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading COD marker\n");
        return OPJ_FALSE;
    }

    /* Apply the coding style to other components of the current tile or the m_default_tcp*/
    opj_j2k_copy_tile_component_parameters(p_j2k);

    /* Index */
#ifdef WIP_REMOVE_MSD
    if (p_j2k->cstr_info) {
        /*opj_codestream_info_t *l_cstr_info = p_j2k->cstr_info;*/
        p_j2k->cstr_info->prog = l_tcp->prg;
        p_j2k->cstr_info->numlayers = l_tcp->numlayers;
        p_j2k->cstr_info->numdecompos = (OPJ_INT32*) opj_malloc(
                                            l_image->numcomps * sizeof(OPJ_UINT32));
        if (!p_j2k->cstr_info->numdecompos) {
            return OPJ_FALSE;
        }
        for (i = 0; i < l_image->numcomps; ++i) {
            p_j2k->cstr_info->numdecompos[i] = l_tcp->tccps[i].numresolutions - 1;
        }
    }
#endif

    return OPJ_TRUE;
}