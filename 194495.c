circ_array_set(CircArray ca, VALUE obj, unsigned long id) {
    if (0 < id) {
	unsigned long	i;

	if (ca->size < id) {
	    unsigned long	cnt = id + 512;

	    if (ca->objs == ca->obj_array) {
		ca->objs = ALLOC_N(VALUE, cnt);
		memcpy(ca->objs, ca->obj_array, sizeof(VALUE) * ca->cnt);
	    } else {
		REALLOC_N(ca->objs, VALUE, cnt);
	    }
	    ca->size = cnt;
	}
	id--;
	for (i = ca->cnt; i < id; i++) {
	    ca->objs[i] = Qundef;
	}
	ca->objs[id] = obj;
	if (ca->cnt <= id) {
	    ca->cnt = id + 1;
	}
    }
}