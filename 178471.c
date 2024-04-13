vim_regfree(regprog_T *prog)
{
    if (prog != NULL)
	prog->engine->regfree(prog);
}