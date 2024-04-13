void TDStretch::calcSeqParameters()
{
    // Adjust tempo param according to tempo, so that variating processing sequence length is used
    // at various tempo settings, between the given low...top limits
    #define AUTOSEQ_TEMPO_LOW   0.5     // auto setting low tempo range (-50%)
    #define AUTOSEQ_TEMPO_TOP   2.0     // auto setting top tempo range (+100%)

    // sequence-ms setting values at above low & top tempo
    #define AUTOSEQ_AT_MIN      90.0
    #define AUTOSEQ_AT_MAX      40.0
    #define AUTOSEQ_K           ((AUTOSEQ_AT_MAX - AUTOSEQ_AT_MIN) / (AUTOSEQ_TEMPO_TOP - AUTOSEQ_TEMPO_LOW))
    #define AUTOSEQ_C           (AUTOSEQ_AT_MIN - (AUTOSEQ_K) * (AUTOSEQ_TEMPO_LOW))

    // seek-window-ms setting values at above low & top tempoq
    #define AUTOSEEK_AT_MIN     20.0
    #define AUTOSEEK_AT_MAX     15.0
    #define AUTOSEEK_K          ((AUTOSEEK_AT_MAX - AUTOSEEK_AT_MIN) / (AUTOSEQ_TEMPO_TOP - AUTOSEQ_TEMPO_LOW))
    #define AUTOSEEK_C          (AUTOSEEK_AT_MIN - (AUTOSEEK_K) * (AUTOSEQ_TEMPO_LOW))

    #define CHECK_LIMITS(x, mi, ma) (((x) < (mi)) ? (mi) : (((x) > (ma)) ? (ma) : (x)))

    double seq, seek;
    
    if (bAutoSeqSetting)
    {
        seq = AUTOSEQ_C + AUTOSEQ_K * tempo;
        seq = CHECK_LIMITS(seq, AUTOSEQ_AT_MAX, AUTOSEQ_AT_MIN);
        sequenceMs = (int)(seq + 0.5);
    }

    if (bAutoSeekSetting)
    {
        seek = AUTOSEEK_C + AUTOSEEK_K * tempo;
        seek = CHECK_LIMITS(seek, AUTOSEEK_AT_MAX, AUTOSEEK_AT_MIN);
        seekWindowMs = (int)(seek + 0.5);
    }

    // Update seek window lengths
    seekWindowLength = (sampleRate * sequenceMs) / 1000;
    if (seekWindowLength < 2 * overlapLength) 
    {
        seekWindowLength = 2 * overlapLength;
    }
    seekLength = (sampleRate * seekWindowMs) / 1000;
}