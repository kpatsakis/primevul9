static SQInteger array_extend(HSQUIRRELVM v)
{
    _array(stack_get(v,1))->Extend(_array(stack_get(v,2)));
    sq_pop(v,1);
    return 1;
}