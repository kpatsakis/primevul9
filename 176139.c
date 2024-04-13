OPJ_BOOL opj_j2k_get_tile(opj_j2k_t *p_j2k,
                          opj_stream_private_t *p_stream,
                          opj_image_t* p_image,
                          opj_event_mgr_t * p_manager,
                          OPJ_UINT32 tile_index)
{
    OPJ_UINT32 compno;
    OPJ_UINT32 l_tile_x, l_tile_y;
    opj_image_comp_t* l_img_comp;

    if (!p_image) {
        opj_event_msg(p_manager, EVT_ERROR, "We need an image previously created.\n");
        return OPJ_FALSE;
    }

    if (/*(tile_index < 0) &&*/ (tile_index >= p_j2k->m_cp.tw * p_j2k->m_cp.th)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Tile index provided by the user is incorrect %d (max = %d) \n", tile_index,
                      (p_j2k->m_cp.tw * p_j2k->m_cp.th) - 1);
        return OPJ_FALSE;
    }

    /* Compute the dimension of the desired tile*/
    l_tile_x = tile_index % p_j2k->m_cp.tw;
    l_tile_y = tile_index / p_j2k->m_cp.tw;

    p_image->x0 = l_tile_x * p_j2k->m_cp.tdx + p_j2k->m_cp.tx0;
    if (p_image->x0 < p_j2k->m_private_image->x0) {
        p_image->x0 = p_j2k->m_private_image->x0;
    }
    p_image->x1 = (l_tile_x + 1) * p_j2k->m_cp.tdx + p_j2k->m_cp.tx0;
    if (p_image->x1 > p_j2k->m_private_image->x1) {
        p_image->x1 = p_j2k->m_private_image->x1;
    }

    p_image->y0 = l_tile_y * p_j2k->m_cp.tdy + p_j2k->m_cp.ty0;
    if (p_image->y0 < p_j2k->m_private_image->y0) {
        p_image->y0 = p_j2k->m_private_image->y0;
    }
    p_image->y1 = (l_tile_y + 1) * p_j2k->m_cp.tdy + p_j2k->m_cp.ty0;
    if (p_image->y1 > p_j2k->m_private_image->y1) {
        p_image->y1 = p_j2k->m_private_image->y1;
    }

    l_img_comp = p_image->comps;
    for (compno = 0; compno < p_image->numcomps; ++compno) {
        OPJ_INT32 l_comp_x1, l_comp_y1;

        l_img_comp->factor = p_j2k->m_private_image->comps[compno].factor;

        l_img_comp->x0 = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)p_image->x0,
                         (OPJ_INT32)l_img_comp->dx);
        l_img_comp->y0 = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)p_image->y0,
                         (OPJ_INT32)l_img_comp->dy);
        l_comp_x1 = opj_int_ceildiv((OPJ_INT32)p_image->x1, (OPJ_INT32)l_img_comp->dx);
        l_comp_y1 = opj_int_ceildiv((OPJ_INT32)p_image->y1, (OPJ_INT32)l_img_comp->dy);

        l_img_comp->w = (OPJ_UINT32)(opj_int_ceildivpow2(l_comp_x1,
                                     (OPJ_INT32)l_img_comp->factor) - opj_int_ceildivpow2((OPJ_INT32)l_img_comp->x0,
                                             (OPJ_INT32)l_img_comp->factor));
        l_img_comp->h = (OPJ_UINT32)(opj_int_ceildivpow2(l_comp_y1,
                                     (OPJ_INT32)l_img_comp->factor) - opj_int_ceildivpow2((OPJ_INT32)l_img_comp->y0,
                                             (OPJ_INT32)l_img_comp->factor));

        l_img_comp++;
    }

    /* Destroy the previous output image*/
    if (p_j2k->m_output_image) {
        opj_image_destroy(p_j2k->m_output_image);
    }

    /* Create the ouput image from the information previously computed*/
    p_j2k->m_output_image = opj_image_create0();
    if (!(p_j2k->m_output_image)) {
        return OPJ_FALSE;
    }
    opj_copy_image_header(p_image, p_j2k->m_output_image);

    p_j2k->m_specific_param.m_decoder.m_tile_ind_to_dec = (OPJ_INT32)tile_index;

    /* customization of the decoding */
    if (!opj_j2k_setup_decoding_tile(p_j2k, p_manager)) {
        return OPJ_FALSE;
    }

    /* Decode the codestream */
    if (! opj_j2k_exec(p_j2k, p_j2k->m_procedure_list, p_stream, p_manager)) {
        opj_image_destroy(p_j2k->m_private_image);
        p_j2k->m_private_image = NULL;
        return OPJ_FALSE;
    }

    /* Move data and copy one information from codec to output image*/
    for (compno = 0; compno < p_image->numcomps; compno++) {
        p_image->comps[compno].resno_decoded =
            p_j2k->m_output_image->comps[compno].resno_decoded;

        if (p_image->comps[compno].data) {
            opj_image_data_free(p_image->comps[compno].data);
        }

        p_image->comps[compno].data = p_j2k->m_output_image->comps[compno].data;

        p_j2k->m_output_image->comps[compno].data = NULL;
    }

    return OPJ_TRUE;
}