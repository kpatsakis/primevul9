static bool _hsort(HSQUIRRELVM v,SQObjectPtr &arr, SQInteger SQ_UNUSED_ARG(l), SQInteger SQ_UNUSED_ARG(r),SQInteger func)
{
    SQArray *a = _array(arr);
    SQInteger i;
    SQInteger array_size = a->Size();
    for (i = (array_size / 2); i >= 0; i--) {
        if(!_hsort_sift_down(v,a, i, array_size - 1,func)) return false;
    }

    for (i = array_size-1; i >= 1; i--)
    {
        _Swap(a->_values[0],a->_values[i]);
        if(!_hsort_sift_down(v,a, 0, i-1,func)) return false;
    }
    return true;
}