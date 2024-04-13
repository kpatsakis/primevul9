FreeWild(int count, char_u **files)
{
    if (count <= 0 || files == NULL)
	return;
    while (count--)
	vim_free(files[count]);
    vim_free(files);
}