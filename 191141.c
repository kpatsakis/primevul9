gs_push_string(gs_main_instance * minst, byte * chars, uint length,
               bool read_only)
{
    ref vref;

    make_string(&vref, avm_foreign | (read_only ? a_readonly : a_all),
                length, (byte *) chars);
    return push_value(minst, &vref);
}