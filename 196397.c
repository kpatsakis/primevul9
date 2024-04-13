Formattable::Formattable(UObject* objectToAdopt)
{
    init();
    fType = kObject;
    fValue.fObject = objectToAdopt;
}