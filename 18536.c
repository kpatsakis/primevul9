SECU_GetPasswordString(void *arg UNUSED, char *prompt)
{
	char *ret;
	ingress();
	ret = get_password(stdin, stdout, prompt, check_password);
	egress();
	return ret;
}