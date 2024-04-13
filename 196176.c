DecimalQuantity::DecimalQuantity(DecimalQuantity&& src) U_NOEXCEPT {
    *this = std::move(src);
}