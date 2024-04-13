Formattable::adoptObject(UObject* objectToAdopt) {
    dispose();
    fType = kObject;
    fValue.fObject = objectToAdopt;
}