#include <arduino.h>
#include <SoftwareSerial.h>

// Pin 4: Vcc （5V)
// Pin 3: GND

// Using SoftSerial: （3.3V logical)
//   Pin 2: int8_t swsTX （RX on MCU)
//   Pin 1: int8_t swsRX  (TX on MCU, may not connect)

// Data updates every 1 second.

class Smt8404 {
private:
  uint16_t HCHOdata = 0;
  uint8_t digiNum = 0;
  const byte sig_op = 0xFF;
  unsigned long timeout;
  SoftwareSerial* _smt8404Serial;
  static const decltype(timeout) timeoutPassive = 68;
  uint8_t CheckSum(uint8_t *i, uint8_t ln);
public:
	enum smt8404Status : uint8_t {
		OK = 0,
		noData,
		readError,
		frameLenMismatch,
		sumError,
    nValues_SmtStatus,
  };

  static const char *errorMsg[nValues_SmtStatus];

  void setTimeout(const decltype(timeout) timeout);
	decltype(timeout) getTimeout(void) const;
	static const auto wakeupTime = 5000U;    // Experimentally, time to get ready after reset/wakeup
  static constexpr const char* Metric = "mg/m3";
  static constexpr const char* Type = "HCHO";

  Smt8404(int8_t swsRX, int8_t swsTX);
  Smt8404(void);
	~Smt8404();
	bool begin(void);
	void end(void);
	size_t available(void);
	smt8404Status read(void);
  bool waitForData(const unsigned int maxTime, const size_t nData = 0);
  uint16_t getHCHO(void);
  uint8_t getDigi(void);
  float getHCHOFloat(void);
};
