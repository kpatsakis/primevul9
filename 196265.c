void DecimalQuantity::roundToMagnitude(int32_t magnitude, RoundingMode roundingMode, bool nickel, UErrorCode& status) {
    // The position in the BCD at which rounding will be performed; digits to the right of position
    // will be rounded away.
    int position = safeSubtract(magnitude, scale);

    // "trailing" = least significant digit to the left of rounding
    int8_t trailingDigit = getDigitPos(position);

    if (position <= 0 && !isApproximate && (!nickel || trailingDigit == 0 || trailingDigit == 5)) {
        // All digits are to the left of the rounding magnitude.
    } else if (precision == 0) {
        // No rounding for zero.
    } else {
        // Perform rounding logic.
        // "leading" = most significant digit to the right of rounding
        int8_t leadingDigit = getDigitPos(safeSubtract(position, 1));

        // Compute which section of the number we are in.
        // EDGE means we are at the bottom or top edge, like 1.000 or 1.999 (used by doubles)
        // LOWER means we are between the bottom edge and the midpoint, like 1.391
        // MIDPOINT means we are exactly in the middle, like 1.500
        // UPPER means we are between the midpoint and the top edge, like 1.916
        roundingutils::Section section;
        if (!isApproximate) {
            if (nickel && trailingDigit != 2 && trailingDigit != 7) {
                // Nickel rounding, and not at .02x or .07x
                if (trailingDigit < 2) {
                    // .00, .01 => down to .00
                    section = roundingutils::SECTION_LOWER;
                } else if (trailingDigit < 5) {
                    // .03, .04 => up to .05
                    section = roundingutils::SECTION_UPPER;
                } else if (trailingDigit < 7) {
                    // .05, .06 => down to .05
                    section = roundingutils::SECTION_LOWER;
                } else {
                    // .08, .09 => up to .10
                    section = roundingutils::SECTION_UPPER;
                }
            } else if (leadingDigit < 5) {
                // Includes nickel rounding .020-.024 and .070-.074
                section = roundingutils::SECTION_LOWER;
            } else if (leadingDigit > 5) {
                // Includes nickel rounding .026-.029 and .076-.079
                section = roundingutils::SECTION_UPPER;
            } else {
                // Includes nickel rounding .025 and .075
                section = roundingutils::SECTION_MIDPOINT;
                for (int p = safeSubtract(position, 2); p >= 0; p--) {
                    if (getDigitPos(p) != 0) {
                        section = roundingutils::SECTION_UPPER;
                        break;
                    }
                }
            }
        } else {
            int32_t p = safeSubtract(position, 2);
            int32_t minP = uprv_max(0, precision - 14);
            if (leadingDigit == 0 && (!nickel || trailingDigit == 0 || trailingDigit == 5)) {
                section = roundingutils::SECTION_LOWER_EDGE;
                for (; p >= minP; p--) {
                    if (getDigitPos(p) != 0) {
                        section = roundingutils::SECTION_LOWER;
                        break;
                    }
                }
            } else if (leadingDigit == 4 && (!nickel || trailingDigit == 2 || trailingDigit == 7)) {
                section = roundingutils::SECTION_MIDPOINT;
                for (; p >= minP; p--) {
                    if (getDigitPos(p) != 9) {
                        section = roundingutils::SECTION_LOWER;
                        break;
                    }
                }
            } else if (leadingDigit == 5 && (!nickel || trailingDigit == 2 || trailingDigit == 7)) {
                section = roundingutils::SECTION_MIDPOINT;
                for (; p >= minP; p--) {
                    if (getDigitPos(p) != 0) {
                        section = roundingutils::SECTION_UPPER;
                        break;
                    }
                }
            } else if (leadingDigit == 9 && (!nickel || trailingDigit == 4 || trailingDigit == 9)) {
                section = roundingutils::SECTION_UPPER_EDGE;
                for (; p >= minP; p--) {
                    if (getDigitPos(p) != 9) {
                        section = roundingutils::SECTION_UPPER;
                        break;
                    }
                }
            } else if (nickel && trailingDigit != 2 && trailingDigit != 7) {
                // Nickel rounding, and not at .02x or .07x
                if (trailingDigit < 2) {
                    // .00, .01 => down to .00
                    section = roundingutils::SECTION_LOWER;
                } else if (trailingDigit < 5) {
                    // .03, .04 => up to .05
                    section = roundingutils::SECTION_UPPER;
                } else if (trailingDigit < 7) {
                    // .05, .06 => down to .05
                    section = roundingutils::SECTION_LOWER;
                } else {
                    // .08, .09 => up to .10
                    section = roundingutils::SECTION_UPPER;
                }
            } else if (leadingDigit < 5) {
                // Includes nickel rounding .020-.024 and .070-.074
                section = roundingutils::SECTION_LOWER;
            } else {
                // Includes nickel rounding .026-.029 and .076-.079
                section = roundingutils::SECTION_UPPER;
            }

            bool roundsAtMidpoint = roundingutils::roundsAtMidpoint(roundingMode);
            if (safeSubtract(position, 1) < precision - 14 ||
                (roundsAtMidpoint && section == roundingutils::SECTION_MIDPOINT) ||
                (!roundsAtMidpoint && section < 0 /* i.e. at upper or lower edge */)) {
                // Oops! This means that we have to get the exact representation of the double,
                // because the zone of uncertainty is along the rounding boundary.
                convertToAccurateDouble();
                roundToMagnitude(magnitude, roundingMode, nickel, status); // start over
                return;
            }

            // Turn off the approximate double flag, since the value is now confirmed to be exact.
            isApproximate = false;
            origDouble = 0.0;
            origDelta = 0;

            if (position <= 0 && (!nickel || trailingDigit == 0 || trailingDigit == 5)) {
                // All digits are to the left of the rounding magnitude.
                return;
            }

            // Good to continue rounding.
            if (section == -1) { section = roundingutils::SECTION_LOWER; }
            if (section == -2) { section = roundingutils::SECTION_UPPER; }
        }

        // Nickel rounding "half even" goes to the nearest whole (away from the 5).
        bool isEven = nickel
                ? (trailingDigit < 2 || trailingDigit > 7
                        || (trailingDigit == 2 && section != roundingutils::SECTION_UPPER)
                        || (trailingDigit == 7 && section == roundingutils::SECTION_UPPER))
                : (trailingDigit % 2) == 0;

        bool roundDown = roundingutils::getRoundingDirection(isEven,
                isNegative(),
                section,
                roundingMode,
                status);
        if (U_FAILURE(status)) {
            return;
        }

        // Perform truncation
        if (position >= precision) {
            setBcdToZero();
            scale = magnitude;
        } else {
            shiftRight(position);
        }

        if (nickel) {
            if (trailingDigit < 5 && roundDown) {
                setDigitPos(0, 0);
                compact();
                return;
            } else if (trailingDigit >= 5 && !roundDown) {
                setDigitPos(0, 9);
                trailingDigit = 9;
                // do not return: use the bubbling logic below
            } else {
                setDigitPos(0, 5);
                // compact not necessary: digit at position 0 is nonzero
                return;
            }
        }

        // Bubble the result to the higher digits
        if (!roundDown) {
            if (trailingDigit == 9) {
                int bubblePos = 0;
                // Note: in the long implementation, the most digits BCD can have at this point is
                // 15, so bubblePos <= 15 and getDigitPos(bubblePos) is safe.
                for (; getDigitPos(bubblePos) == 9; bubblePos++) {}
                shiftRight(bubblePos); // shift off the trailing 9s
            }
            int8_t digit0 = getDigitPos(0);
            U_ASSERT(digit0 != 9);
            setDigitPos(0, static_cast<int8_t>(digit0 + 1));
            precision += 1; // in case an extra digit got added
        }

        compact();
    }
}