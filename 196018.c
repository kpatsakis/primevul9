static void netfilter_print_info(Monitor *mon, NetFilterState *nf)
{
    char *str;
    ObjectProperty *prop;
    ObjectPropertyIterator iter;
    Visitor *v;

    /* generate info str */
    object_property_iter_init(&iter, OBJECT(nf));
    while ((prop = object_property_iter_next(&iter))) {
        if (!strcmp(prop->name, "type")) {
            continue;
        }
        v = string_output_visitor_new(false, &str);
        object_property_get(OBJECT(nf), prop->name, v, NULL);
        visit_complete(v, &str);
        visit_free(v);
        monitor_printf(mon, ",%s=%s", prop->name, str);
        g_free(str);
    }
    monitor_printf(mon, "\n");
}