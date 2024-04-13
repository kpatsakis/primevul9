parse_vw(
    XOC		oc,
    FontSet	font_set,
    char	**name_list,
    int		count)
{
    FontData	vmap = font_set->vmap;
    VRotate	vrotate = font_set->vrotate;
    int		vmap_num = font_set->vmap_num;
    int		vrotate_num = font_set->vrotate_num;
    int		ret = 0, i = 0;

    if(vmap_num > 0) {
	if(parse_fontdata(oc, font_set, vmap, vmap_num, name_list,
			  count, C_VMAP,NULL) == -1)
	    return (-1);
    }

    if(vrotate_num > 0) {
	ret = parse_fontdata(oc, font_set, (FontData) vrotate, vrotate_num,
			     name_list, count, C_VROTATE, NULL);
	if(ret == -1) {
	    return (-1);
	} else if(ret == False) {
	    CodeRange	code_range;
	    int		num_cr;
	    int		sub_num = font_set->substitute_num;

	    code_range = vrotate[0].code_range; /* ? */
	    num_cr = vrotate[0].num_cr;         /* ? */
	    for(i = 0 ; i < vrotate_num ; i++) {
		if(vrotate[i].xlfd_name)
		    Xfree(vrotate[i].xlfd_name);
	    }
	    Xfree(vrotate);

	    if(sub_num > 0) {
		vrotate = font_set->vrotate = Xcalloc(sub_num,
                                                      sizeof(VRotateRec));
		if(font_set->vrotate == (VRotate)NULL)
		    return (-1);

		for(i = 0 ; i < sub_num ; i++) {
		    vrotate[i].charset_name = font_set->substitute[i].name;
		    vrotate[i].side = font_set->substitute[i].side;
		    vrotate[i].code_range = code_range;
		    vrotate[i].num_cr = num_cr;
		}
		vrotate_num = font_set->vrotate_num = sub_num;
	    } else {
		vrotate = font_set->vrotate = (VRotate)NULL;
	    }

	    ret = parse_fontdata(oc, font_set, (FontData) vrotate, vrotate_num,
				 name_list, count, C_VROTATE, NULL);
	    if(ret == -1)
		return (-1);
	}
    }

    return True;
}