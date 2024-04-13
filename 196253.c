void DecimalQuantity::roundToInfinity() {
    if (isApproximate) {
        convertToAccurateDouble();
    }
}