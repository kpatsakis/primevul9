static SQInteger array_remove(HSQUIRRELVM v)
{
    SQObject &o = stack_get(v, 1);
    SQObject &idx = stack_get(v, 2);
    if(!sq_isnumeric(idx)) return sq_throwerror(v, _SC("wrong type"));
    SQObjectPtr val;
    if(_array(o)->Get(tointeger(idx), val)) {
        _array(o)->Remove(tointeger(idx));
        v->Push(val);
        return 1;
    }
    return sq_throwerror(v, _SC("idx out of range"));
}