init_fontdata(
    FontData	font_data,
    int		font_data_count)
{
    FontData	fd;
    int		i;

    fd = Xcalloc(font_data_count, sizeof(FontDataRec));
    if(fd == (FontData) NULL)
	return False;

    for(i = 0 ; i < font_data_count ; i++)
	fd[i] = font_data[i];

    return fd;
}