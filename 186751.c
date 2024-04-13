create_fontset(
    XOC oc)
{
    XOMGenericPart *gen = XOM_GENERIC(oc->core.om);
    int found_num;

    if (init_fontset(oc) == False)
        return False;

    found_num = parse_fontname(oc);
    if (found_num <= 0) {
	if (found_num == 0)
	    set_missing_list(oc);
	return False;
    }

    if (gen->on_demand_loading == True) {
	if (load_font_info(oc) == False)
	    return False;
    } else {
	if (load_font(oc) == False)
	    return False;
    }

    if (init_core_part(oc) == False)
	return False;

    if (set_missing_list(oc) == False)
	return False;

    return True;
}