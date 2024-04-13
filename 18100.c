static bool str2num(const SQChar *s,SQObjectPtr &res,SQInteger base)
{
    SQChar *end;
    const SQChar *e = s;
    bool iseintbase = base > 13; //to fix error converting hexadecimals with e like 56f0791e
    bool isfloat = false;
    SQChar c;
    while((c = *e) != _SC('\0'))
    {
        if (c == _SC('.') || (!iseintbase && (c == _SC('E') || c == _SC('e')))) { //e and E is for scientific notation
            isfloat = true;
            break;
        }
        e++;
    }
    if(isfloat){
        SQFloat r = SQFloat(scstrtod(s,&end));
        if(s == end) return false;
        res = r;
    }
    else{
        SQInteger r = SQInteger(scstrtol(s,&end,(int)base));
        if(s == end) return false;
        res = r;
    }
    return true;
}