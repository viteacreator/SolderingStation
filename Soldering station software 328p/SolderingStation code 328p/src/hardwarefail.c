/*
 * hardwarefail.c
 *
 * Created: 3/4/2023 10:01:28 PM
 *  Author: Victor
 */ 
class hardwarefail
{
	public:
	hardwarefail() {}
	
	void begin(int16_t floatRealTemp) {
		_floatRealTemp = floatRealTemp;
	}
	
	bool checkforfail(int16_t realTemp, int16_t setTemp) {
		if (_lastSetTemp > setTemp) {
			_flagDownTemp = 1;
			_flagUpTemp = 0;
			_lastSetTemp = setTemp;
		}
		if (_lastSetTemp < setTemp) {
			_flagDownTemp = 0;
			_flagUpTemp = 1;
			_lastSetTemp = setTemp;
		}
		if (_flagUpTemp and _floatRealTemp < realTemp) _floatRealTemp = realTemp;
		if (_flagDownTemp and _floatRealTemp > realTemp) _floatRealTemp = realTemp;
		if (_floatRealTemp == _lastSetTemp) {
			_flagDownTemp = 0;
			_flagUpTemp = 0;
		}
		if (_floatRealTemp < realTemp - 50 or _floatRealTemp > realTemp + 50) {
			return true;
		}
		return false;
	}

	private:
	int16_t _lastSetTemp, _floatRealTemp;
	bool _flagDownTemp = 0, _flagUpTemp = 0;
};
