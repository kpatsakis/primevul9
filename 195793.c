static int parseLSig(struct cli_bc *bc, char *buffer)
{
    const char *prefix;
    char *vnames, *vend = strchr(buffer, ';');
    if (vend) {
	bc->lsig = cli_strdup(buffer);
	*vend++ = '\0';
	prefix = buffer;
	vnames = strchr(vend, '{');
    } else {
	/* Not a logical signature, but we still have a virusname */
	bc->lsig = NULL;
    }

    return CL_SUCCESS;
}