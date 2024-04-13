gs_push_real(gs_main_instance * minst, double value)
{
    ref vref;

    make_real(&vref, value);
    return push_value(minst, &vref);
}