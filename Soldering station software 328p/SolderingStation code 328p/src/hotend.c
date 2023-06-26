/*
 * hotend.c
 *
 * Created: 3/4/2023 10:01:02 PM
 *  Author: Victor
 */ 
//---------------------------------------------- HotEnd class ---------------------------------------------------
class HotEnd
{ //---------------------------------- Methods --------------------------------------
	public:
	HotEnd(uint8_t heat_coil_pin, uint8_t termocuple_pin)
	{
		_heat_coil_pin = heat_coil_pin;
		_termocuple_pin = termocuple_pin;

		pinMode(_heat_coil_pin, OUTPUT);
		digitalWrite(_heat_coil_pin, LOW);

		pinMode(_termocuple_pin, INPUT);
	};
	//    ~HotEnd(){};

	int16_t readTemp();
	int16_t median(int16_t newVal);
	float expRunningAverage(float newVal);
	void liniarTemp();

	void setTemp();
	void setTempCrude(uint16_t);
	void setTempZero();

	uint8_t PID();
	uint8_t PID(uint16_t);
	
	void setPID(float kp, float ki, float kd) {
		setPID(kp, ki, kd, 150);
	};
	void setPID(float kp, float ki, float kd, uint8_t dt) {
		_kp = kp; _ki = ki; _kd = kd; _dt = dt;
	};

	int16_t getRealTemp() {
		return _real_temp;
	};
	
	void setSetTemp(int16_t settemp) {
		_set_temp = settemp;
	};
	int16_t getSetTemp() {
		return _set_temp;
	};
	
	void setState(bool state) {
		_state = state;
	};
	bool getState() {
		return _state;
	};
	int8_t get_dt() {
		return _dt;
	};
	//------------------------ temp liniar func set-get ------------------
	void setLiniarTemp(int16_t inMin, int16_t inMax, int16_t outMin, int16_t outMax) {
		_inMin = inMin; _inMax = inMax; _outMin = outMin; _outMax = outMax;
	};
	int16_t getInMin() {
		return _inMin;
	};
	int16_t getInMax() {
		return _inMax;
	};
	int16_t getOutMin() {
		return _outMin;
	};
	void setOutMin(int16_t t) {
		_outMin = t;
	};
	int16_t getOutMax() {
		return _outMax;
	};
	void setOutMax(int16_t t) {
		_outMax = t;
	};
	//------------------------ temp liniar func set-get ------------------
	//---------------------------------- Methods --------------------------------------

	//-------------------------------- Parameters -------------------------------------
	protected:
	int16_t _real_temp;
	int16_t _set_temp;
	bool _state;

	// expRunningAverage parameter
	float _k = 0.9;        // 0.0 - 1.0
	float _filVal = 0;

	// median filter var
	int _buf[3];
	byte _count = 0;

	// PID parameter
	uint8_t _dt = 150; // 150 ms
	float _kp;
	float _ki;
	float _kd;
	
	float _integral = 0, _prevErr = 0;
	uint8_t _min = 0, _max = _dt;       // _max = _dt because see void HotEnd::setTemp()

	// 328p pins
	uint8_t _heat_coil_pin;
	uint8_t _termocuple_pin;

	// setTemp var
	uint32_t _prev_setTemp;
	uint8_t _setPwm;

	// Liniar function param
	int16_t _inMin = 200;
	int16_t _inMax = 400;
	int16_t _outMin = 200; //208;
	int16_t _outMax = 400; //410;
	//-------------------------------- Parameters -------------------------------------
};//-------------------------------------------- HotEnd class ---------------------------------------------------

//-------------------------------------------- HotEnd methodes --------------------------------------------------

//------------------------------- Read actual temp ----------------------------------
int16_t HotEnd::readTemp()
{
	int16_t a = (analogRead(_termocuple_pin) / 2);
	a = median(a);
	_real_temp = expRunningAverage(a);
	return _real_temp;
}
//------------------------------- Read actual temp ----------------------------------

//------------------------------- Filter algorithm ----------------------------------
int16_t HotEnd::median(int16_t newVal)
{
	_buf[_count] = newVal;
	if (++_count >= 3) _count = 0;
	return (max(_buf[0], _buf[1]) == max(_buf[1], _buf[2])) ? max(_buf[0], _buf[2]) : max(_buf[1], min(_buf[0], _buf[2]));
}

float HotEnd::expRunningAverage(float newVal)
{
	_filVal += (newVal - _filVal) * _k;
	return _filVal;
}
//------------------------------- Filter algorithm ----------------------------------

//---------------------------- Liniar temp algorithm --------------------------------
void HotEnd::liniarTemp()
{
	_real_temp = constrain(map(_real_temp, _inMin, _inMax, _outMin, _outMax), 0, 550);
}
//---------------------------- Liniar temp algorithm --------------------------------

//----------------------------------- Set temp --------------------------------------
void HotEnd::setTemp()
{
	if (millis() - _prev_setTemp > _dt) {
		readTemp();
		liniarTemp();
		//    analogWrite(_heat_coil_pin, PID());
		_setPwm = PID();
		_prev_setTemp = millis();
	}
	//---------------------------------------
	if (millis() - _prev_setTemp < _setPwm) {
		digitalWrite(_heat_coil_pin, HIGH);
		} else {
		digitalWrite(_heat_coil_pin, LOW);
	}
	//---------------------------------------
}

void HotEnd::setTempCrude(uint16_t real_temp)                 // Set temp without liniar algorithm
{
	if (millis() - _prev_setTemp > _dt) {
		readTemp();
		//    analogWrite(_heat_coil_pin, PID());
		_setPwm = PID(real_temp);
		_prev_setTemp = millis();
	}
	//---------------------------------------
	if (millis() - _prev_setTemp < _setPwm) {
		digitalWrite(_heat_coil_pin, HIGH);
		} else {
		digitalWrite(_heat_coil_pin, LOW);
	}
	//---------------------------------------
}

void HotEnd::setTempZero()
{
	digitalWrite(_heat_coil_pin, LOW);
}
//----------------------------------- Set temp --------------------------------------

//------------------------------------- PID -----------------------------------------
uint8_t HotEnd::PID()
{
	return PID(_real_temp);
}
uint8_t HotEnd::PID(uint16_t real_temp)
{
	float dt = (float)_dt / 1000;
	float input = real_temp;
	float setpoint = _set_temp;
	float err = setpoint - input;
	_integral = constrain(_integral + (float)err * dt * _ki, _min, _max);
	float D = (err - _prevErr) / dt;
	_prevErr = err;
	//    Serial.println(err);
	//    Serial.println(_kp);
	//    Serial.println(_integral);
	//    Serial.println(_ki);
	//    Serial.println(D);
	//    Serial.println(_kd);
	return constrain(err * _kp + _integral + D * _kd, _min, _max);;
}//------------------------------------- PID -----------------------------------------

//-------------------------------------------- HotEnd methodes --------------------------------------------------

//---------------------------------------------- HotAir class ---------------------------------------------------

class HotAir : public HotEnd

{ //---------------------------------- Methods --------------------------------------
	public:
	HotAir(uint8_t heat_coil_pin, uint8_t termocuple_pin, uint8_t fan_pin, uint8_t reed_sw_pin): HotEnd(heat_coil_pin, termocuple_pin)
	{
		_fan_pin = fan_pin;
		_reed_sw_pin = reed_sw_pin;

		pinMode(_fan_pin, OUTPUT);
		digitalWrite(_fan_pin, LOW);

		pinMode(_reed_sw_pin, INPUT_PULLUP);
	};
	//    ~HotAir(){};

	void setTemp();
	void setTempZero();

	bool getReedSW() {
		return digitalRead(_reed_sw_pin);
	};
	uint8_t getFan() {
		return _fan;
	};
	void setFan(uint8_t fan) {
		_fan = fan;
	};
	void runFan() {
		analogWrite(_fan_pin, (_fan * 12 + 147));
	};
	void runFanFull() {
		digitalWrite(_fan_pin, HIGH);
	};
	//---------------------------------- Methods --------------------------------------

	//-------------------------------- Parameters -------------------------------------
	private:
	uint8_t _fan;

	uint8_t _reed_sw_pin;
	uint8_t _fan_pin;
	//-------------------------------- Parameters -------------------------------------

};//-------------------------------------------- HotAir class ---------------------------------------------------

//-------------------------------------------- HotAir methodes --------------------------------------------------

//----------------------------------- Set temp --------------------------------------
void HotAir::setTemp()
{
	if (getReedSW()) {                                                          // If reed switch is not actuated...
		HotEnd::setTemp();                                                        // ...setTemp
		runFan();                                                                 // and run fan
		} else {
		setTempZero();                                                            // Otherwise set temp zero
	}
}

void HotAir::setTempZero()
{
	if (millis() - _prev_setTemp > _dt) {                                       // Once in _dt time...
		readTemp();                                                               // ... read actual temp
		_prev_setTemp = millis();
	}
	if (getRealTemp() > 50) {                                                   // If temp more 50*C
		runFanFull();                                                             // Turn on the fan
		} else {
		digitalWrite(_fan_pin, LOW);                                              // Otherwise turn off the fan
	}
}
//----------------------------------- Set temp --------------------------------------

//-------------------------------------------- HotAir methodes --------------------------------------------------
