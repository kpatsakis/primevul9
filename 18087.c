static SQInteger __getcallstackinfos(HSQUIRRELVM v,SQInteger level)
{
    SQStackInfos si;
    SQInteger seq = 0;
    const SQChar *name = NULL;

    if (SQ_SUCCEEDED(sq_stackinfos(v, level, &si)))
    {
        const SQChar *fn = _SC("unknown");
        const SQChar *src = _SC("unknown");
        if(si.funcname)fn = si.funcname;
        if(si.source)src = si.source;
        sq_newtable(v);
        sq_pushstring(v, _SC("func"), -1);
        sq_pushstring(v, fn, -1);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, _SC("src"), -1);
        sq_pushstring(v, src, -1);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, _SC("line"), -1);
        sq_pushinteger(v, si.line);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, _SC("locals"), -1);
        sq_newtable(v);
        seq=0;
        while ((name = sq_getlocal(v, level, seq))) {
            sq_pushstring(v, name, -1);
            sq_push(v, -2);
            sq_newslot(v, -4, SQFalse);
            sq_pop(v, 1);
            seq++;
        }
        sq_newslot(v, -3, SQFalse);
        return 1;
    }

    return 0;
}