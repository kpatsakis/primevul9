Formattable::adoptDecimalQuantity(DecimalQuantity *dq) {
    if (fDecimalQuantity != NULL) {
        delete fDecimalQuantity;
    }
    fDecimalQuantity = dq;
    if (dq == NULL) { // allow adoptDigitList(NULL) to clear
        return;
    }

    // Set the value into the Union of simple type values.
    // Cannot use the set() functions because they would delete the fDecimalNum value.
    if (fDecimalQuantity->fitsInLong()) {
        fValue.fInt64 = fDecimalQuantity->toLong();
        if (fValue.fInt64 <= INT32_MAX && fValue.fInt64 >= INT32_MIN) {
            fType = kLong;
        } else {
            fType = kInt64;
        }
    } else {
        fType = kDouble;
        fValue.fDouble = fDecimalQuantity->toDouble();
    }
}