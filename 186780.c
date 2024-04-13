_XInitOM(
    XLCd lcd)
{
    lcd->methods->open_om = _XomGenericOpenOM;

    return True;
}