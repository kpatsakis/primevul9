pdf_render_mode_uses_stroke(const gx_device_pdf *pdev,
                            const pdf_text_state_values_t *ptsv)
{
    return ((ptsv->render_mode == 1 || ptsv->render_mode == 2 ||
            ptsv->render_mode == 5 || ptsv->render_mode == 6));
}