int SafeAdd(size_t* res, size_t a, size_t b)
{
    if ( !psnip_safe_size_add(res, a, b) ) {
        *res = 0;
        return 1;
    }

    return 0;
}