gs_push_integer(gs_main_instance * minst, long value)
{
    ref vref;

    make_int(&vref, value);
    return push_value(minst, &vref);
}