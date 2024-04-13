static SQInteger array_sort(HSQUIRRELVM v)
{
    SQInteger func = -1;
    SQObjectPtr &o = stack_get(v,1);
    if(_array(o)->Size() > 1) {
        if(sq_gettop(v) == 2) func = 2;
        if(!_hsort(v, o, 0, _array(o)->Size()-1, func))
            return SQ_ERROR;

    }
    sq_settop(v,1);
    return 1;
}