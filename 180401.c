nogvl_ffi_call(void *ptr)
{
    struct nogvl_ffi_call_args *args = ptr;

    ffi_call(args->cif, args->fn, &args->retval, args->values);

    return NULL;
}