static int __init parse_trust_cpu(char *arg)
{
	return kstrtobool(arg, &trust_cpu);
}