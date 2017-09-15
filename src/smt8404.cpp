#include <smt8404.h>

void smt8404::setTimeout(const decltype(timeout) timeout) {
  this->_dartSerial->setTimeout(timeout);
  this->timeout = timeout;
};

decltype(smt8404::timeout) smt8404::getTimeout(void) const {
  return timeout;
};

smt8404::smt8404(int8_t swsRX, int8_t swsTX) {
  this->_dartSerial = new SoftwareSerial(swsRX, swsTX);
};

smt8404::~smt8404() {

};

bool smt8404::begin(void) {
  this->_dartSerial->setTimeout(smt8404::timeoutPassive);
  this->_dartSerial->begin(9600);
  return true;
};

void smt8404::end(void) {
  this->_dartSerial->end();
};

size_t smt8404::available(void) {
  while (this->_dartSerial->available()) {
    if (this->_dartSerial->peek() != sig_op) {
      this->_dartSerial->read();
    } else {
      break;
    }
  }
  return static_cast<size_t>(this->_dartSerial->available());
}


smt8404::dartStatus smt8404::read() {

  if (available() < 9) {
    //Serial.println("noDATA");
    return noData;
  }
  //Serial.println("Try read:");
  uint8_t thisFrame[9];
  
  if (this->_dartSerial->readBytes(thisFrame, sizeof(thisFrame)) != sizeof(thisFrame)) {
    //Serial.println(sizeof(thisFrame));
    return frameLenMismatch;
  };

  if (thisFrame[0] != smt8404::sig_op) {
    //Serial.println("sig_op Error");
    return readError;
  }

  byte checkbit = CheckSum(thisFrame, 9);
  if ( thisFrame[8] != checkbit ) {
    // Serial.print("SumError: Exp=");
    // Serial.print(thisFrame[8]);
    // Serial.print(" Act=");
    // Serial.println(checkbit);
    return sumError;
  }
  
  this->HCHOdata = thisFrame[4] * 256 + thisFrame[5];
  this->digiNum = thisFrame[3];

  return OK;
}

bool smt8404::waitForData(const unsigned int maxTime, const size_t nData) {
  const auto t0 = millis();
  if (nData == 0) {
    for (; (millis() - t0) < maxTime; delay(1)) {
      if (this->_dartSerial->available()) {
        return true;
      }
    }
    return this->_dartSerial->available();
  }

  for (; (millis() - t0) < maxTime; delay(1)) {
    if (available() >= nData) {
      return true;
    }
  }
  return available() >= nData;
}

uint16_t smt8404::getHCHO(void) {
  return this->HCHOdata;
}

uint8_t smt8404::getDigi(void) {
  return this->digiNum;
}

float smt8404::getHCHOFloat(void) {
  float multival=1.0;
  for (int8_t i=0;i<this->digiNum;++i) {
    multival *= 0.1;
  }
  return this->HCHOdata * multival;
}

uint8_t smt8404::CheckSum(uint8_t *i, uint8_t ln) {
  uint8_t j, tempq=0;
  i+=1;
  for(j=0;j<(ln-2);++j) {
    tempq+=*i;++i;
  }
  tempq = (~tempq) + 1;
  return tempq;
}