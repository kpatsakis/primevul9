OPJ_BOOL opj_j2k_decode(opj_j2k_t * p_j2k,
                        opj_stream_private_t * p_stream,
                        opj_image_t * p_image,
                        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 compno;

    if (!p_image) {
        return OPJ_FALSE;
    }

    p_j2k->m_output_image = opj_image_create0();
    if (!(p_j2k->m_output_image)) {
        return OPJ_FALSE;
    }
    opj_copy_image_header(p_image, p_j2k->m_output_image);

    /* customization of the decoding */
    if (!opj_j2k_setup_decoding(p_j2k, p_manager)) {
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
        p_image->comps[compno].data = p_j2k->m_output_image->comps[compno].data;
#if 0
        char fn[256];
        sprintf(fn, "/tmp/%d.raw", compno);
        FILE *debug = fopen(fn, "wb");
        fwrite(p_image->comps[compno].data, sizeof(OPJ_INT32),
               p_image->comps[compno].w * p_image->comps[compno].h, debug);
        fclose(debug);
#endif
        p_j2k->m_output_image->comps[compno].data = NULL;
    }

    return OPJ_TRUE;
}