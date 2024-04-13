bool pdf_compare_text_state_for_charpath(pdf_text_state_t *pts, gx_device_pdf *pdev,
                                         gs_gstate *pgs, gs_font *font,
                                         const gs_text_params_t *text)
{
    int code;
    float size;
    gs_matrix smat, tmat;
    struct pdf_font_resource_s *pdfont;

    /* check to ensure the new text has the same length as the saved text */
    if(text->size != pts->buffer.count_chars)
        return(false);

    if(font->FontType == ft_user_defined ||
        font->FontType == ft_PCL_user_defined ||
        font->FontType == ft_MicroType ||
        font->FontType == ft_GL2_stick_user_defined ||
        font->FontType == ft_GL2_531)
        return(false);

    /* check to ensure the new text has the same data as the saved text */
    if(memcmp(text->data.bytes, &pts->buffer.chars, text->size))
        return(false);

    /* See if the same font is in use by checking the attahced pdfont resource for
     * the currrent font and comparing with the saved text state
     */
    code = pdf_attached_font_resource(pdev, font, &pdfont, NULL, NULL, NULL, NULL);
    if(code < 0)
        return(false);

    if(!pdfont || pdfont != pts->in.pdfont)
        return(false);

    /* Check to see the new text starts at the same point as the saved text.
     * NB! only check 2 decimal places, allow some slack in the match. This
     * still may prove to be too tight a requirement.
     */
    if(fabs(pts->start.x - pgs->current_point.x) > 0.01 ||
       fabs(pts->start.y - pgs->current_point.y) > 0.01)
        return(false);

    size = pdf_calculate_text_size(pgs, pdfont, &font->FontMatrix, &smat, &tmat, font, pdev);

    /* Finally, check the calculated size against the size stored in
     * the text state.
     */
    if(size != pts->in.size)
        return(false);

    return(true);
}