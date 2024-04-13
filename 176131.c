void opj_j2k_destroy(opj_j2k_t *p_j2k)
{
    if (p_j2k == 00) {
        return;
    }

    if (p_j2k->m_is_decoder) {

        if (p_j2k->m_specific_param.m_decoder.m_default_tcp != 00) {
            opj_j2k_tcp_destroy(p_j2k->m_specific_param.m_decoder.m_default_tcp);
            opj_free(p_j2k->m_specific_param.m_decoder.m_default_tcp);
            p_j2k->m_specific_param.m_decoder.m_default_tcp = 00;
        }

        if (p_j2k->m_specific_param.m_decoder.m_header_data != 00) {
            opj_free(p_j2k->m_specific_param.m_decoder.m_header_data);
            p_j2k->m_specific_param.m_decoder.m_header_data = 00;
            p_j2k->m_specific_param.m_decoder.m_header_data_size = 0;
        }
    } else {

        if (p_j2k->m_specific_param.m_encoder.m_encoded_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_encoded_tile_data);
            p_j2k->m_specific_param.m_encoder.m_encoded_tile_data = 00;
        }

        if (p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer);
            p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer = 00;
            p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current = 00;
        }

        if (p_j2k->m_specific_param.m_encoder.m_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = 00;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
        }
    }

    opj_tcd_destroy(p_j2k->m_tcd);

    opj_j2k_cp_destroy(&(p_j2k->m_cp));
    memset(&(p_j2k->m_cp), 0, sizeof(opj_cp_t));

    opj_procedure_list_destroy(p_j2k->m_procedure_list);
    p_j2k->m_procedure_list = 00;

    opj_procedure_list_destroy(p_j2k->m_validation_list);
    p_j2k->m_procedure_list = 00;

    j2k_destroy_cstr_index(p_j2k->cstr_index);
    p_j2k->cstr_index = NULL;

    opj_image_destroy(p_j2k->m_private_image);
    p_j2k->m_private_image = NULL;

    opj_image_destroy(p_j2k->m_output_image);
    p_j2k->m_output_image = NULL;

    opj_thread_pool_destroy(p_j2k->m_tp);
    p_j2k->m_tp = NULL;

    opj_free(p_j2k);
}