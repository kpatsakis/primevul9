static OPJ_BOOL opj_j2k_check_poc_val(const opj_poc_t *p_pocs,
                                      OPJ_UINT32 p_nb_pocs,
                                      OPJ_UINT32 p_nb_resolutions,
                                      OPJ_UINT32 p_num_comps,
                                      OPJ_UINT32 p_num_layers,
                                      opj_event_mgr_t * p_manager)
{
    OPJ_UINT32* packet_array;
    OPJ_UINT32 index, resno, compno, layno;
    OPJ_UINT32 i;
    OPJ_UINT32 step_c = 1;
    OPJ_UINT32 step_r = p_num_comps * step_c;
    OPJ_UINT32 step_l = p_nb_resolutions * step_r;
    OPJ_BOOL loss = OPJ_FALSE;
    OPJ_UINT32 layno0 = 0;

    packet_array = (OPJ_UINT32*) opj_calloc(step_l * p_num_layers,
                                            sizeof(OPJ_UINT32));
    if (packet_array == 00) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory for checking the poc values.\n");
        return OPJ_FALSE;
    }

    if (p_nb_pocs == 0) {
        opj_free(packet_array);
        return OPJ_TRUE;
    }

    index = step_r * p_pocs->resno0;
    /* take each resolution for each poc */
    for (resno = p_pocs->resno0 ; resno < p_pocs->resno1 ; ++resno) {
        OPJ_UINT32 res_index = index + p_pocs->compno0 * step_c;

        /* take each comp of each resolution for each poc */
        for (compno = p_pocs->compno0 ; compno < p_pocs->compno1 ; ++compno) {
            OPJ_UINT32 comp_index = res_index + layno0 * step_l;

            /* and finally take each layer of each res of ... */
            for (layno = layno0; layno < p_pocs->layno1 ; ++layno) {
                /*index = step_r * resno + step_c * compno + step_l * layno;*/
                packet_array[comp_index] = 1;
                comp_index += step_l;
            }

            res_index += step_c;
        }

        index += step_r;
    }
    ++p_pocs;

    /* iterate through all the pocs */
    for (i = 1; i < p_nb_pocs ; ++i) {
        OPJ_UINT32 l_last_layno1 = (p_pocs - 1)->layno1 ;

        layno0 = (p_pocs->layno1 > l_last_layno1) ? l_last_layno1 : 0;
        index = step_r * p_pocs->resno0;

        /* take each resolution for each poc */
        for (resno = p_pocs->resno0 ; resno < p_pocs->resno1 ; ++resno) {
            OPJ_UINT32 res_index = index + p_pocs->compno0 * step_c;

            /* take each comp of each resolution for each poc */
            for (compno = p_pocs->compno0 ; compno < p_pocs->compno1 ; ++compno) {
                OPJ_UINT32 comp_index = res_index + layno0 * step_l;

                /* and finally take each layer of each res of ... */
                for (layno = layno0; layno < p_pocs->layno1 ; ++layno) {
                    /*index = step_r * resno + step_c * compno + step_l * layno;*/
                    packet_array[comp_index] = 1;
                    comp_index += step_l;
                }

                res_index += step_c;
            }

            index += step_r;
        }

        ++p_pocs;
    }

    index = 0;
    for (layno = 0; layno < p_num_layers ; ++layno) {
        for (resno = 0; resno < p_nb_resolutions; ++resno) {
            for (compno = 0; compno < p_num_comps; ++compno) {
                loss |= (packet_array[index] != 1);
                /*index = step_r * resno + step_c * compno + step_l * layno;*/
                index += step_c;
            }
        }
    }

    if (loss) {
        opj_event_msg(p_manager, EVT_ERROR, "Missing packets possible loss of data\n");
    }

    opj_free(packet_array);

    return !loss;
}