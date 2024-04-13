DecimalQuantity &DecimalQuantity::operator=(const DecimalQuantity &other) {
    if (this == &other) {
        return *this;
    }
    copyBcdFrom(other);
    copyFieldsFrom(other);
    return *this;
}