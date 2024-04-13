void DecimalQuantity::truncate() {
    if (scale < 0) {
        shiftRight(-scale);
        scale = 0;
        compact();
    }
}