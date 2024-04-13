free_fontdataOM(
    FontData	font_data,
    int		font_data_count)
{
    if (!font_data)
	return;

    for( ; font_data_count-- ; font_data++) {
	    Xfree(font_data->name);
	    font_data->name = NULL;
	    Xfree(font_data->scopes);
	    font_data->scopes = NULL;
    }
}