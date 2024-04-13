init_vrotate(
    FontData	font_data,
    int		font_data_count,
    int		type,
    CodeRange	code_range,
    int		code_range_num)
{
    VRotate	vrotate;
    int		i;

    if(type == VROTATE_NONE)
	return (VRotate)NULL;

    vrotate = Xcalloc(font_data_count, sizeof(VRotateRec));
    if(vrotate == (VRotate) NULL)
	return False;

    for(i = 0 ; i < font_data_count ; i++) {
	vrotate[i].charset_name = font_data[i].name;
	vrotate[i].side = font_data[i].side;
	if(type == VROTATE_PART) {
	    vrotate[i].num_cr = code_range_num;
	    vrotate[i].code_range = code_range;
	}
    }

    return vrotate;
}