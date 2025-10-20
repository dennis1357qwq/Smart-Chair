#include "tof_manager.h"

ToFManager::ToFManager() {}

bool ToFManager::add(const ToF& tof) {
	if (_count >= MAX_TOF) return false;
  	if (!tof.mux) return false;

  	_tofs[_count] = tof;
  	_tofs[_count].valid = false;
  	_tofs[_count].l0 = nullptr;
  	_tofs[_count].l1 = nullptr;

  	_count++;
  	return true;
}

void ToFManager::init() {
	_l0xN = 0;
	_l1xN = 0;
  	for (uint8_t i = 0; i < _count; ++i) {
    	auto& tof = _tofs[i];
		tof.valid = false;
    	tof.l0 = nullptr;
    	tof.l1 = nullptr;
    	tof.mux->select(tof.muxCh);

		switch (tof.type) {
			case ToFType::L0X: {
				if(_l0xN >= MAX_L0X) { 
					tof.valid = false; 
					continue; 
				}
				auto* sensor = &_l0xPool[_l0xN++];
				sensor->setTimeout(100);                         
				tof.valid = sensor->init();
				if (tof.valid) {
					sensor->setMeasurementTimingBudget(50000);   
					(void)sensor->readRangeSingleMillimeters();
					(void)sensor->readRangeSingleMillimeters();
					delay(60);
				}
				tof.l0 = sensor; tof.l1 = nullptr;
			} break;

			case ToFType::L1X: {
				if (_l1xN >= MAX_L1X) {
					tof.valid = false;
					continue;
				}
				auto* sensor = &_l1xPool[_l1xN++];
				sensor->setTimeout(200);
				tof.valid = sensor->init();
				if (tof.valid) {
					sensor->setDistanceMode(VL53L1X::Medium);
					sensor->setMeasurementTimingBudget(33000);
					sensor->startContinuous(33); // non-blocking reads
					delay(60);
				}
				tof.l0 = nullptr;
				tof.l1 = sensor;
			} break;
		}
  	}
}

void ToFManager::update(Telemetry& t) {
	for (uint8_t i = 0; i < _count; ++i) {
		auto& tof = _tofs[i];
		if (!tof.valid) continue;

		tof.mux->select(tof.muxCh);
		delayMicroseconds(300);
		int value = -1;

		switch (tof.type) {
		case ToFType::L0X: {
			uint16_t mm = tof.l0->readRangeSingleMillimeters();
			if (!tof.l0->timeoutOccurred() && mm != 8191) value = (int)mm;
		} break;

		case ToFType::L1X: {
			if (tof.l1->dataReady()) {
			uint16_t mm = tof.l1->read(false); // non-blocking
			value = (int)mm;
			// optional tof.l1->clearInterrupt();
			}
		} break;
		}

		writeToTelemetry(tof, t, value);
	}
}

void ToFManager::writeToTelemetry(const ToF& tof, Telemetry& t, int value) {
	switch (tof.slot) {
		case ToFSlot::BACK:
		if (tof.idx < TofData::BACK_N) t.tof.back[tof.idx] = value; break;
		case ToFSlot::HEAD:
		if (tof.idx < TofData::HEAD_N) t.tof.head[tof.idx] = value; break;
		case ToFSlot::KNEE:
		if (tof.idx < TofData::KNEE_N) t.tof.knee[tof.idx] = value; break;
		case ToFSlot::BOTTOM:
		if (tof.idx < TofData::BOTTOM_N) t.tof.bottom[tof.idx] = value; break;
	}
}