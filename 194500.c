circ_array_get(CircArray ca, unsigned long id) {
    VALUE	obj = Qundef;

    if (id <= ca->cnt) {
	obj = ca->objs[id - 1];
    }
    return obj;
}