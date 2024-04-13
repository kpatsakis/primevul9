static SQInteger array_resize(HSQUIRRELVM v)
{
    SQObject &o = stack_get(v, 1);
    SQObject &nsize = stack_get(v, 2);
    SQObjectPtr fill;
    if(sq_isnumeric(nsize)) {
        SQInteger sz = tointeger(nsize);
        if (sz<0)
          return sq_throwerror(v, _SC("resizing to negative length"));

        if(sq_gettop(v) > 2)
            fill = stack_get(v, 3);
        _array(o)->Resize(sz,fill);
        sq_settop(v, 1);
        return 1;
    }
    return sq_throwerror(v, _SC("size must be a number"));
}