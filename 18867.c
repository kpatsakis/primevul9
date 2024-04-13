blink_state_is_inverted()
{
#ifdef FEAT_TERMRESPONSE
    return rbm_status.tr_progress == STATUS_GOT
	&& rcs_status.tr_progress == STATUS_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
#else
    return FALSE;
#endif
}