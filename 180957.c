static int __init cmdline_parse_movablecore(char *p)
{
	return cmdline_parse_core(p, &required_movablecore);
}