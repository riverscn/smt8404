#include <smt8404.h>

void Smt8404::setTimeout(const decltype(timeout) timeout) {
  this->_smt8404Serial->setTimeout(timeout);
  this->timeout = timeout;
};

decltype(Smt8404::timeout) Smt8404::getTimeout(void) const {
  return timeout;
};

Smt8404::Smt8404(int8_t swsRX, int8_t swsTX) {
  this->_smt8404Serial = new SoftwareSerial(swsRX, swsTX);
};

Smt8404::~Smt8404() {

};

bool Smt8404::begin(void) {
  this->_smt8404Serial->setTimeout(Smt8404::timeoutPassive);
  this->_smt8404Serial->begin(9600);
  return true;
};

void Smt8404::end(void) {
  this->_smt8404Serial->end();
};

size_t Smt8404::available(void) {
  while (this->_smt8404Serial->available()) {
    if (this->_smt8404Serial->peek() != sig_op) {
      this->_smt8404Serial->read();
    } else {
      break;
    }
  }
  return static_cast<size_t>(this->_smt8404Serial->available());
}


Smt8404::smt8404Status Smt8404::read() {

  if (available() < 9) {
    return noData;
  }
  uint8_t thisFrame[9];
  
  if (this->_smt8404Serial->readBytes(thisFrame, sizeof(thisFrame)) != sizeof(thisFrame)) {
    return frameLenMismatch;
  };

  if (thisFrame[0] != Smt8404::sig_op) {
    return readError;
  }

  byte checkbit = CheckSum(thisFrame, 9);
  if ( thisFrame[8] != checkbit ) {
    return sumError;
  }
  
  this->HCHOdata = thisFrame[4] * 256 + thisFrame[5];
  this->digiNum = thisFrame[3];

  return OK;
}

bool Smt8404::waitForData(const unsigned int maxTime, const size_t nData) {
  const auto t0 = millis();
  if (nData == 0) {
    for (; (millis() - t0) < maxTime; delay(1)) {
      if (this->_smt8404Serial->available()) {
        return true;
      }
    }
    return this->_smt8404Serial->available();
  }

  for (; (millis() - t0) < maxTime; delay(1)) {
    if (available() >= nData) {
      return true;
    }
  }
  return available() >= nData;
}

uint16_t Smt8404::getHCHO(void) {
  return this->HCHOdata;
}

uint8_t Smt8404::getDigi(void) {
  return this->digiNum;
}

float Smt8404::getHCHOFloat(void) {
  float multival=1.0;
  for (int8_t i=0;i<this->digiNum;++i) {
    multival *= 0.1;
  }
  return this->HCHOdata * multival;
}

uint8_t Smt8404::CheckSum(uint8_t *i, uint8_t ln) {
  uint8_t j, tempq=0;
  i+=1;
  for(j=0;j<(ln-2);++j) {
    tempq+=*i;++i;
  }
  tempq = (~tempq) + 1;
  return tempq;
}