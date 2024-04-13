pdf_reset_text_state(pdf_text_data_t *ptd)
{
    pdf_text_state_t *pts = ptd->text_state;

    pts->out = ts_default.out;
    pts->leading = 0;
}