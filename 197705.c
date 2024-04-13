pdf_set_text_state_values(gx_device_pdf *pdev,
                          const pdf_text_state_values_t *ptsv)
{
    pdf_text_state_t *pts = pdev->text->text_state;

    if (pts->buffer.count_chars > 0) {
        int code;

        if (pts->in.character_spacing == ptsv->character_spacing &&
            pts->in.pdfont == ptsv->pdfont && pts->in.size == ptsv->size &&
            pts->in.render_mode == ptsv->render_mode &&
            pts->in.word_spacing == ptsv->word_spacing
            ) {
            if (!gs_matrix_compare(&pts->in.matrix, &ptsv->matrix))
                return 0;
            /* add_text_delta_move sets pts->in.matrix if successful */
            code = add_text_delta_move(pdev, &ptsv->matrix);
            if (code >= 0)
                return 0;
        }
        code = sync_text_state(pdev);
        if (code < 0)
            return code;
    }

    pts->in = *ptsv;
    pts->continue_line = false;
    return 0;
}