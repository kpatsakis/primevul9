limit_screen_size(void)
{
    if (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    else if (Columns > 10000)
	Columns = 10000;
    if (Rows > 1000)
	Rows = 1000;
}