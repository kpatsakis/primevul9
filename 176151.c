static OPJ_BOOL opj_j2k_read_sot(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager)
{
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_tot_len, l_num_parts = 0;
    OPJ_UINT32 l_current_part;
    OPJ_UINT32 l_tile_x, l_tile_y;

    /* preconditions */

    assert(p_j2k != 00);
    assert(p_manager != 00);

    if (! opj_j2k_get_sot_values(p_header_data, p_header_size,
                                 &(p_j2k->m_current_tile_number), &l_tot_len, &l_current_part, &l_num_parts,
                                 p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading SOT marker\n");
        return OPJ_FALSE;
    }

    l_cp = &(p_j2k->m_cp);

    /* testcase 2.pdf.SIGFPE.706.1112 */
    if (p_j2k->m_current_tile_number >= l_cp->tw * l_cp->th) {
        opj_event_msg(p_manager, EVT_ERROR, "Invalid tile number %d\n",
                      p_j2k->m_current_tile_number);
        return OPJ_FALSE;
    }

    l_tcp = &l_cp->tcps[p_j2k->m_current_tile_number];
    l_tile_x = p_j2k->m_current_tile_number % l_cp->tw;
    l_tile_y = p_j2k->m_current_tile_number / l_cp->tw;

    /* Fixes issue with id_000020,sig_06,src_001958,op_flip4,pos_149 */
    /* of https://github.com/uclouvain/openjpeg/issues/939 */
    /* We must avoid reading twice the same tile part number for a given tile */
    /* so as to avoid various issues, like opj_j2k_merge_ppt being called */
    /* several times. */
    /* ISO 15444-1 A.4.2 Start of tile-part (SOT) mandates that tile parts */
    /* should appear in increasing order. */
    if (l_tcp->m_current_tile_part_number + 1 != (OPJ_INT32)l_current_part) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Invalid tile part index for tile number %d. "
                      "Got %d, expected %d\n",
                      p_j2k->m_current_tile_number,
                      l_current_part,
                      l_tcp->m_current_tile_part_number + 1);
        return OPJ_FALSE;
    }
    ++ l_tcp->m_current_tile_part_number;

#ifdef USE_JPWL
    if (l_cp->correct) {

        OPJ_UINT32 tileno = p_j2k->m_current_tile_number;
        static OPJ_UINT32 backup_tileno = 0;

        /* tileno is negative or larger than the number of tiles!!! */
        if (tileno > (l_cp->tw * l_cp->th)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "JPWL: bad tile number (%d out of a maximum of %d)\n",
                          tileno, (l_cp->tw * l_cp->th));
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
            /* we try to correct */
            tileno = backup_tileno;
            opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust this\n"
                          "- setting tile number to %d\n",
                          tileno);
        }

        /* keep your private count of tiles */
        backup_tileno++;
    };
#endif /* USE_JPWL */

    /* look for the tile in the list of already processed tile (in parts). */
    /* Optimization possible here with a more complex data structure and with the removing of tiles */
    /* since the time taken by this function can only grow at the time */

    /* PSot should be equal to zero or >=14 or <= 2^32-1 */
    if ((l_tot_len != 0) && (l_tot_len < 14)) {
        if (l_tot_len ==
                12) { /* MSD: Special case for the PHR data which are read by kakadu*/
            opj_event_msg(p_manager, EVT_WARNING, "Empty SOT marker detected: Psot=%d.\n",
                          l_tot_len);
        } else {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Psot value is not correct regards to the JPEG2000 norm: %d.\n", l_tot_len);
            return OPJ_FALSE;
        }
    }

#ifdef USE_JPWL
    if (l_cp->correct) {

        /* totlen is negative or larger than the bytes left!!! */
        if (/*(l_tot_len < 0) ||*/ (l_tot_len >
                                    p_header_size)) {   /* FIXME it seems correct; for info in V1 -> (p_stream_numbytesleft(p_stream) + 8))) { */
            opj_event_msg(p_manager, EVT_ERROR,
                          "JPWL: bad tile byte size (%d bytes against %d bytes left)\n",
                          l_tot_len,
                          p_header_size);  /* FIXME it seems correct; for info in V1 -> p_stream_numbytesleft(p_stream) + 8); */
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
            /* we try to correct */
            l_tot_len = 0;
            opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust this\n"
                          "- setting Psot to %d => assuming it is the last tile\n",
                          l_tot_len);
        }
    };
#endif /* USE_JPWL */

    /* Ref A.4.2: Psot could be equal zero if it is the last tile-part of the codestream.*/
    if (!l_tot_len) {
        opj_event_msg(p_manager, EVT_INFO,
                      "Psot value of the current tile-part is equal to zero, "
                      "we assuming it is the last tile-part of the codestream.\n");
        p_j2k->m_specific_param.m_decoder.m_last_tile_part = 1;
    }

    if (l_tcp->m_nb_tile_parts != 0 && l_current_part >= l_tcp->m_nb_tile_parts) {
        /* Fixes https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=2851 */
        opj_event_msg(p_manager, EVT_ERROR,
                      "In SOT marker, TPSot (%d) is not valid regards to the previous "
                      "number of tile-part (%d), giving up\n", l_current_part,
                      l_tcp->m_nb_tile_parts);
        p_j2k->m_specific_param.m_decoder.m_last_tile_part = 1;
        return OPJ_FALSE;
    }

    if (l_num_parts !=
            0) { /* Number of tile-part header is provided by this tile-part header */
        l_num_parts += p_j2k->m_specific_param.m_decoder.m_nb_tile_parts_correction;
        /* Useful to manage the case of textGBR.jp2 file because two values of TNSot are allowed: the correct numbers of
         * tile-parts for that tile and zero (A.4.2 of 15444-1 : 2002). */
        if (l_tcp->m_nb_tile_parts) {
            if (l_current_part >= l_tcp->m_nb_tile_parts) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "In SOT marker, TPSot (%d) is not valid regards to the current "
                              "number of tile-part (%d), giving up\n", l_current_part,
                              l_tcp->m_nb_tile_parts);
                p_j2k->m_specific_param.m_decoder.m_last_tile_part = 1;
                return OPJ_FALSE;
            }
        }
        if (l_current_part >= l_num_parts) {
            /* testcase 451.pdf.SIGSEGV.ce9.3723 */
            opj_event_msg(p_manager, EVT_ERROR,
                          "In SOT marker, TPSot (%d) is not valid regards to the current "
                          "number of tile-part (header) (%d), giving up\n", l_current_part, l_num_parts);
            p_j2k->m_specific_param.m_decoder.m_last_tile_part = 1;
            return OPJ_FALSE;
        }
        l_tcp->m_nb_tile_parts = l_num_parts;
    }

    /* If know the number of tile part header we will check if we didn't read the last*/
    if (l_tcp->m_nb_tile_parts) {
        if (l_tcp->m_nb_tile_parts == (l_current_part + 1)) {
            p_j2k->m_specific_param.m_decoder.m_can_decode =
                1; /* Process the last tile-part header*/
        }
    }

    if (!p_j2k->m_specific_param.m_decoder.m_last_tile_part) {
        /* Keep the size of data to skip after this marker */
        p_j2k->m_specific_param.m_decoder.m_sot_length = l_tot_len -
                12; /* SOT_marker_size = 12 */
    } else {
        /* FIXME: need to be computed from the number of bytes remaining in the codestream */
        p_j2k->m_specific_param.m_decoder.m_sot_length = 0;
    }

    p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_TPH;

    /* Check if the current tile is outside the area we want decode or not corresponding to the tile index*/
    if (p_j2k->m_specific_param.m_decoder.m_tile_ind_to_dec == -1) {
        p_j2k->m_specific_param.m_decoder.m_skip_data =
            (l_tile_x < p_j2k->m_specific_param.m_decoder.m_start_tile_x)
            || (l_tile_x >= p_j2k->m_specific_param.m_decoder.m_end_tile_x)
            || (l_tile_y < p_j2k->m_specific_param.m_decoder.m_start_tile_y)
            || (l_tile_y >= p_j2k->m_specific_param.m_decoder.m_end_tile_y);
    } else {
        assert(p_j2k->m_specific_param.m_decoder.m_tile_ind_to_dec >= 0);
        p_j2k->m_specific_param.m_decoder.m_skip_data =
            (p_j2k->m_current_tile_number != (OPJ_UINT32)
             p_j2k->m_specific_param.m_decoder.m_tile_ind_to_dec);
    }

    /* Index */
    if (p_j2k->cstr_index) {
        assert(p_j2k->cstr_index->tile_index != 00);
        p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tileno =
            p_j2k->m_current_tile_number;
        p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_tpsno =
            l_current_part;

        if (l_num_parts != 0) {
            p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].nb_tps =
                l_num_parts;
            p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps =
                l_num_parts;

            if (!p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index) {
                p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index =
                    (opj_tp_index_t*)opj_calloc(l_num_parts, sizeof(opj_tp_index_t));
                if (!p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index) {
                    opj_event_msg(p_manager, EVT_ERROR,
                                  "Not enough memory to read SOT marker. Tile index allocation failed\n");
                    return OPJ_FALSE;
                }
            } else {
                opj_tp_index_t *new_tp_index = (opj_tp_index_t *) opj_realloc(
                                                   p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index,
                                                   l_num_parts * sizeof(opj_tp_index_t));
                if (! new_tp_index) {
                    opj_free(p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index);
                    p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index = NULL;
                    opj_event_msg(p_manager, EVT_ERROR,
                                  "Not enough memory to read SOT marker. Tile index allocation failed\n");
                    return OPJ_FALSE;
                }
                p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index =
                    new_tp_index;
            }
        } else {
            /*if (!p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index)*/ {

                if (!p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index) {
                    p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps = 10;
                    p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index =
                        (opj_tp_index_t*)opj_calloc(
                            p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps,
                            sizeof(opj_tp_index_t));
                    if (!p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index) {
                        p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps = 0;
                        opj_event_msg(p_manager, EVT_ERROR,
                                      "Not enough memory to read SOT marker. Tile index allocation failed\n");
                        return OPJ_FALSE;
                    }
                }

                if (l_current_part >=
                        p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps) {
                    opj_tp_index_t *new_tp_index;
                    p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps =
                        l_current_part + 1;
                    new_tp_index = (opj_tp_index_t *) opj_realloc(
                                       p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index,
                                       p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps *
                                       sizeof(opj_tp_index_t));
                    if (! new_tp_index) {
                        opj_free(p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index);
                        p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index = NULL;
                        p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].current_nb_tps = 0;
                        opj_event_msg(p_manager, EVT_ERROR,
                                      "Not enough memory to read SOT marker. Tile index allocation failed\n");
                        return OPJ_FALSE;
                    }
                    p_j2k->cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index =
                        new_tp_index;
                }
            }

        }

    }

    /* FIXME move this onto a separate method to call before reading any SOT, remove part about main_end header, use a index struct inside p_j2k */
    /* if (p_j2k->cstr_info) {
       if (l_tcp->first) {
       if (tileno == 0) {
       p_j2k->cstr_info->main_head_end = p_stream_tell(p_stream) - 13;
       }

       p_j2k->cstr_info->tile[tileno].tileno = tileno;
       p_j2k->cstr_info->tile[tileno].start_pos = p_stream_tell(p_stream) - 12;
       p_j2k->cstr_info->tile[tileno].end_pos = p_j2k->cstr_info->tile[tileno].start_pos + totlen - 1;
       p_j2k->cstr_info->tile[tileno].num_tps = numparts;

       if (numparts) {
       p_j2k->cstr_info->tile[tileno].tp = (opj_tp_info_t *) opj_malloc(numparts * sizeof(opj_tp_info_t));
       }
       else {
       p_j2k->cstr_info->tile[tileno].tp = (opj_tp_info_t *) opj_malloc(10 * sizeof(opj_tp_info_t)); // Fixme (10)
       }
       }
       else {
       p_j2k->cstr_info->tile[tileno].end_pos += totlen;
       }

       p_j2k->cstr_info->tile[tileno].tp[partno].tp_start_pos = p_stream_tell(p_stream) - 12;
       p_j2k->cstr_info->tile[tileno].tp[partno].tp_end_pos =
       p_j2k->cstr_info->tile[tileno].tp[partno].tp_start_pos + totlen - 1;
       }*/
    return OPJ_TRUE;
}