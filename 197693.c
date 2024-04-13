pdf_close_text_contents(gx_device_pdf *pdev)
{
    /*
     * Clear the font pointer.  This is probably left over from old code,
     * but it is appropriate in case we ever choose in the future to write
     * out and free font resources before the end of the document.
     */
    pdf_text_state_t *pts = pdev->text->text_state;

    pts->in.pdfont = pts->out.pdfont = 0;
    pts->in.size = pts->out.size = 0;
}