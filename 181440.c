	class BadDistanceErr : public Err {public: BadDistanceErr() : Err(INVALID_DATA_FORMAT, "Inflator: error in bit distance") {}};