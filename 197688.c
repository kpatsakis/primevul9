pdf_text_state_alloc(gs_memory_t *mem)
{
    pdf_text_state_t *pts =
        gs_alloc_struct(mem, pdf_text_state_t, &st_pdf_text_state,
                        "pdf_text_state_alloc");

    if (pts == 0)
        return 0;
    *pts = ts_default;
    return pts;
}