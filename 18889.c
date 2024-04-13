check_shellsize(void)
{
    if (Rows < min_rows())	// need room for one window and command line
	Rows = min_rows();
    limit_screen_size();

    // make sure these values are not invalid
    if (cmdline_row >= Rows)
	cmdline_row = Rows - 1;
    if (msg_row >= Rows)
	msg_row = Rows - 1;
}