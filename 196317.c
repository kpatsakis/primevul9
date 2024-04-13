DecimalQuantity& DecimalQuantity::operator=(DecimalQuantity&& src) U_NOEXCEPT {
    if (this == &src) {
        return *this;
    }
    moveBcdFrom(src);
    copyFieldsFrom(src);
    return *this;
}