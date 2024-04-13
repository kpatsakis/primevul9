void TDStretch::adaptNormalizer()
{
    // Do not adapt normalizer over too silent sequences to avoid averaging filter depleting to
    // too low values during pauses in music
    if ((maxnorm > 1000) || (maxnormf > 40000000))
    { 
        //norm averaging filter
        maxnormf = 0.9f * maxnormf + 0.1f * (float)maxnorm;

        if ((maxnorm > 800000000) && (overlapDividerBitsNorm < 16))
        {
            // large values, so increase divider
            overlapDividerBitsNorm++;
            if (maxnorm > 1600000000) overlapDividerBitsNorm++; // extra large value => extra increase
        }
        else if ((maxnormf < 1000000) && (overlapDividerBitsNorm > 0))
        {
            // extra small values, decrease divider
            overlapDividerBitsNorm--;
        }
    }

    maxnorm = 0;
}