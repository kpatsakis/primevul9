init_term_props(int all)
{
    int i;

    term_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    term_props[TPR_CURSOR_STYLE].tpr_set_by_termresponse = FALSE;
    term_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    term_props[TPR_CURSOR_BLINK].tpr_set_by_termresponse = FALSE;
    term_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    term_props[TPR_UNDERLINE_RGB].tpr_set_by_termresponse = TRUE;
    term_props[TPR_MOUSE].tpr_name = "mouse";
    term_props[TPR_MOUSE].tpr_set_by_termresponse = TRUE;

    for (i = 0; i < TPR_COUNT; ++i)
	if (all || term_props[i].tpr_set_by_termresponse)
	    term_props[i].tpr_status = TPR_UNKNOWN;
}