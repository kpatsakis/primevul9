static bool _sort_compare(HSQUIRRELVM v, SQArray *arr, SQObjectPtr &a,SQObjectPtr &b,SQInteger func,SQInteger &ret)
{
    if(func < 0) {
        if(!v->ObjCmp(a,b,ret)) return false;
    }
    else {
        SQInteger top = sq_gettop(v);
        sq_push(v, func);
        sq_pushroottable(v);
        v->Push(a);
        v->Push(b);
		SQObjectPtr *valptr = arr->_values._vals;
		SQUnsignedInteger precallsize = arr->_values.size();
        if(SQ_FAILED(sq_call(v, 3, SQTrue, SQFalse))) {
            if(!sq_isstring( v->_lasterror))
                v->Raise_Error(_SC("compare func failed"));
            return false;
        }
		if(SQ_FAILED(sq_getinteger(v, -1, &ret))) {
            v->Raise_Error(_SC("numeric value expected as return value of the compare function"));
            return false;
        }
		if (precallsize != arr->_values.size() || valptr != arr->_values._vals) {
			v->Raise_Error(_SC("array resized during sort operation"));
			return false;
		}
        sq_settop(v, top);
        return true;
    }
    return true;
}