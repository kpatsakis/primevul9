gs_push_boolean(gs_main_instance * minst, bool value)
{
    ref vref;

    make_bool(&vref, value);
    return push_value(minst, &vref);
}