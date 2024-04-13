void EmitRangeCheck(cmsIOHANDLER* m)
{
    _cmsIOPrintf(m, "dup 0.0 lt { pop 0.0 } if "
                    "dup 1.0 gt { pop 1.0 } if ");

}