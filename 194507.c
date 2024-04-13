circ_array_new() {
    CircArray	ca;
    
    ca = ALLOC(struct _CircArray);
    ca->objs = ca->obj_array;
    ca->size = sizeof(ca->obj_array) / sizeof(VALUE);
    ca->cnt = 0;
    
    return ca;
}