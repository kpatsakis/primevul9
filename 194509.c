circ_array_free(CircArray ca) {
    if (ca->objs != ca->obj_array) {
	xfree(ca->objs);
    }
    xfree(ca);
}