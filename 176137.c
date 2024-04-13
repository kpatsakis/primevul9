static OPJ_BOOL opj_j2k_setup_end_compress(opj_j2k_t *p_j2k,
        opj_event_mgr_t * p_manager)
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);

    /* DEVELOPER CORNER, insert your custom procedures */
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_eoc, p_manager)) {
        return OPJ_FALSE;
    }

    if (OPJ_IS_CINEMA(p_j2k->m_cp.rsiz)) {
        if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                               (opj_procedure)opj_j2k_write_updated_tlm, p_manager)) {
            return OPJ_FALSE;
        }
    }

    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_write_epc, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_end_encoding, p_manager)) {
        return OPJ_FALSE;
    }
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_destroy_header_memory, p_manager)) {
        return OPJ_FALSE;
    }
    return OPJ_TRUE;
}