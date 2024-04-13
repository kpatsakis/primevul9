ascii_is_unsafe(char ch)
{
	return !ch || ch == '\n' || ch == ':';
}