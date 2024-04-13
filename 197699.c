sync_text_state(gx_device_pdf *pdev)
{
    pdf_text_state_t *pts = pdev->text->text_state;
    stream *s = pdev->strm;
    int code;

    if (pts->buffer.count_chars == 0)
        return 0;		/* nothing to output */

    if (pts->continue_line)
        return flush_text_buffer(pdev);

    /* Bring text state parameters up to date. */

    if (pts->out.character_spacing != pts->in.character_spacing) {
        pprintg1(s, "%g Tc\n", pts->in.character_spacing);
        pts->out.character_spacing = pts->in.character_spacing;
    }

    if (pts->out.pdfont != pts->in.pdfont || pts->out.size != pts->in.size) {
        pdf_font_resource_t *pdfont = pts->in.pdfont;

        code = pdf_assign_font_object_id(pdev, pdfont);
        if (code < 0)
            return code;
        pprints1(s, "/%s ", pdfont->rname);
        pprintg1(s, "%g Tf\n", pts->in.size);
        pts->out.pdfont = pdfont;
        pts->out.size = pts->in.size;
        /*
         * In PDF, the only place to specify WMode is in the CMap
         * (a.k.a. Encoding) of a Type 0 font.
         */
        pts->wmode =
            (pdfont->FontType == ft_composite ?
             pdfont->u.type0.WMode : 0);
        code = pdf_used_charproc_resources(pdev, pdfont);
        if (code < 0)
            return code;
    }

    if (gs_matrix_compare(&pts->in.matrix, &pts->out.matrix) ||
         ((pts->start.x != pts->out_pos.x || pts->start.y != pts->out_pos.y) &&
          (pts->buffer.count_chars != 0 || pts->buffer.count_moves != 0))) {
        /* pdf_set_text_matrix sets out.matrix = in.matrix */
        code = pdf_set_text_matrix(pdev);
        if (code < 0)
            return code;
    }

    if (pts->out.render_mode != pts->in.render_mode) {
        pprintg1(s, "%g Tr\n", pts->in.render_mode);
        pts->out.render_mode = pts->in.render_mode;
    }

    if (pts->out.word_spacing != pts->in.word_spacing) {
        if (memchr(pts->buffer.chars, 32, pts->buffer.count_chars)) {
            pprintg1(s, "%g Tw\n", pts->in.word_spacing);
            pts->out.word_spacing = pts->in.word_spacing;
        }
    }

    return flush_text_buffer(pdev);
}