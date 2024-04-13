cursor_on(void)
{
    if (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}