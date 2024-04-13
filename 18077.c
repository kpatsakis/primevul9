static SQInteger table_rawdelete(HSQUIRRELVM v)
{
    if(SQ_FAILED(sq_rawdeleteslot(v,1,SQTrue)))
        return SQ_ERROR;
    return 1;
}