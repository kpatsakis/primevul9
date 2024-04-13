compute_cmdrow(void)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    if (exmode_active || (msg_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    else
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}