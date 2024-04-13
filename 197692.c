int pdf_set_PaintType0_params (gx_device_pdf *pdev, gs_gstate *pgs, float size,
                               double scaled_width, const pdf_text_state_values_t *ptsv)
{
    pdf_text_state_t *pts = pdev->text->text_state;
    double saved_width = pgs->line_params.half_width;
    int code;

    /* This routine is used to check if we have accumulated glyphs waiting for output
     * if we do, and we are using a PaintType 0 font (stroke), which is the only way we
     * can get here, then we check to see if the stroke width has changed. If so we want to
     * flush the buffer, and set the new stroke width. This produces:
     * <width> w
     * (text) Tj
     * <new width> w
     * (new text) Tj
     *
     * instead of :
     * <width> w
     * <new width> w
     * (text) Tj
     * (new text) Tj
     */
    if (pts->buffer.count_chars > 0) {
        if (pts->PaintType0Width != scaled_width) {
            pgs->line_params.half_width = scaled_width / 2;
            code = pdf_set_text_state_values(pdev, ptsv);
            if (code < 0)
                return code;
            if (pdev->text->text_state->in.render_mode == ptsv->render_mode){
                code = pdf_prepare_stroke(pdev, pgs);
                if (code >= 0)
                    code = gdev_vector_prepare_stroke((gx_device_vector *)pdev,
                                              pgs, NULL, NULL, 1);
            }
            if (code < 0)
                return code;
            pgs->line_params.half_width = saved_width;
            pts->PaintType0Width = scaled_width;
        }
    }
    return 0;
}