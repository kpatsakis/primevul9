static void JSON_mark(void *ptr)
{
    JSON_Parser *json = ptr;
    rb_gc_mark_maybe(json->Vsource);
    rb_gc_mark_maybe(json->create_id);
    rb_gc_mark_maybe(json->object_class);
    rb_gc_mark_maybe(json->array_class);
    rb_gc_mark_maybe(json->decimal_class);
    rb_gc_mark_maybe(json->match_string);
}