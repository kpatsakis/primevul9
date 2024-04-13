static SQInteger default_delegate_tofloat(HSQUIRRELVM v)
{
    SQObjectPtr &o=stack_get(v,1);
    switch(sq_type(o)){
    case OT_STRING:{
        SQObjectPtr res;
        if(str2num(_stringval(o),res,10)){
            v->Push(SQObjectPtr(tofloat(res)));
            break;
        }}
        return sq_throwerror(v, _SC("cannot convert the string"));
        break;
    case OT_INTEGER:case OT_FLOAT:
        v->Push(SQObjectPtr(tofloat(o)));
        break;
    case OT_BOOL:
        v->Push(SQObjectPtr((SQFloat)(_integer(o)?1:0)));
        break;
    default:
        v->PushNull();
        break;
    }
    return 1;
}