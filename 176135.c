static OPJ_BOOL opj_j2k_setup_header_writing(opj_j2k_t *p_j2k,
        opj_event_mgr_t * p_manager)
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);

    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_init_info, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_soc, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_siz, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_cod, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_qcd, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_all_coc, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_all_qcc, p_manager)) {
        return OPJ_FALSE;
    }

    if (OPJ_IS_CINEMA(p_j2k->m_cp.rsiz)) {
        if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                               (opj_procedure)opj_j2k_write_tlm, p_manager)) {
            return OPJ_FALSE;
        }

        if (p_j2k->m_cp.rsiz == OPJ_PROFILE_CINEMA_4K) {
            if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                                   (opj_procedure)opj_j2k_write_poc, p_manager)) {
                return OPJ_FALSE;
            }
        }
    }

    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_regions, p_manager)) {
        return OPJ_FALSE;
    }

    if (p_j2k->m_cp.comment != 00)  {
        if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                               (opj_procedure)opj_j2k_write_com, p_manager)) {
            return OPJ_FALSE;
        }
    }

    /* DEVELOPER CORNER, insert your custom procedures */
    if (p_j2k->m_cp.rsiz & OPJ_EXTENSION_MCT) {
        if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                               (opj_procedure)opj_j2k_write_mct_data_group, p_manager)) {
            return OPJ_FALSE;
        }
    }
    /* End of Developer Corner */

    if (p_j2k->cstr_index) {
        if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                               (opj_procedure)opj_j2k_get_end_header, p_manager)) {
            return OPJ_FALSE;
        }
    }

    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_create_tcd, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_update_rates, p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}