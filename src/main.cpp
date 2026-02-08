#include "core/fsr_data.h"
#include "core/posture/baseline/baseline_manager.h"
#include "core/posture/matrix/matrix_posture.h"
#include "core/posture/tof/tof_posture.h"
#include "core/telemetry.h"
#include "core/telemetry_json.h"
#include "hardware/adc/adc_manager.h"
#include "hardware/fsr/fsr_manager.h"
#include "hardware/matrix/matrix_sensor.h"
#include "hardware/mux.h"
#include "hardware/tof/i2c_mux_PCA9548A.h"
#include "hardware/tof/tof_manager.h"
#include <Arduino.h>
#include <Wire.h>
#include <vector>

Telemetry state;
Adc adc;
Mux colMux(25, 33, 32,
           18); // Multiplexer for measurement. Control pins(s0,s1,s2,s3).
Mux rowMux(12, 14, 27, 26); // Multiplexer for Voltage delivery.
PCA9548A i2cMux_1(0x70);
// PCA9548A i2cMux_2(0x71);
ToFManager tofm;
MatrixSensor matrix(rowMux, colMux, 0, adc, 7, 7);
FsrManager fsr(adc, 1, 2, 3);
MatrixPosture matrixPosture(state.matrix.values, MatrixData::ROWS,
                            MatrixData::COLS);
ToFPosture tofPosture(&state.tof);
BaselineManager baselineMgr(matrixPosture, &tofPosture);

void setup() {
  Serial.begin(115200);
  delay(300);

  Wire.begin(21, 22);
  Wire.setClock(100000);
  delay(50);

  i2cMux_1.disableAll();
  delay(5);
  // i2cMux_2.disableAll();
  delay(5);
  adc.initADC();
  matrix.initMatrixSensor();

  tofm.registerMux(i2cMux_1);
  // tofm.registerMux(i2cMux_2);
  tofm.add(ToF(&i2cMux_1, 0, ToFType::L1X, ToFSlot::BACK, 0));
  tofm.add(ToF(&i2cMux_1, 1, ToFType::L1X, ToFSlot::BACK, 1));
  tofm.add(ToF(&i2cMux_1, 2, ToFType::L1X, ToFSlot::BACK, 2));
  tofm.add(ToF(&i2cMux_1, 5, ToFType::L1X, ToFSlot::BACK, 3));
  tofm.add(ToF(&i2cMux_1, 6, ToFType::L1X, ToFSlot::BACK, 4));
  tofm.add(ToF(&i2cMux_1, 7, ToFType::L1X, ToFSlot::BACK, 5));
  // mux 2 for everything non back related
  //  tofm.add(ToF(&i2cMux_2, 7, ToFType::L0X, ToFSlot::KNEE, 0));
  //  tofm.add(ToF(&i2cMux_2, 2, ToFType::L0X, ToFSlot::KNEE, 1));
  //  tofm.add(ToF(&i2cMux_2, 4, ToFType::L1X, ToFSlot::BOTTOM, 0));
  //  tofm.add(ToF(&i2cMux_2, 5, ToFType::L1X, ToFSlot::BOTTOM, 1));
  tofm.init();
}

// void debugPrintMatrixTags(const MatrixTags& tags) {
//   Serial.print("[MatrixTags] ");
//   if (tags.neutral)        Serial.print("neutral ");
//   if (tags.weightLeft)     Serial.print("left ");
//   if (tags.weightRight)    Serial.print("right ");
//   if (tags.weightForward)  Serial.print("forward ");
//   if (tags.weightBackward) Serial.print("backward ");
//   if (tags.crossedLegLeft) Serial.print("xLegL ");
//   if (tags.crossedLegRight)Serial.print("xLegR ");
//   if (tags.sitOnLegLeft)   Serial.print("sitLegL ");
//   if (tags.sitOnLegRight)  Serial.print("sitLegR ");
//   Serial.println();
// }

void debugPrintToFTags(const ToFTags &t) {
  Serial.print("[ToF] ");

  // Präsenz & Neutralität
  Serial.print("backInRange=");
  Serial.print(t.backInRange);
  Serial.print(" neutralBack=");
  Serial.print(t.neutral);

  // Kontakt (irgendeiner / upper / middle / lower)
  Serial.print(" | contactFull=");
  Serial.println(t.contact);

  // Distanz oben
  Serial.print(" | upperDistance(Contact/Slight/Medium/Far)=");
  Serial.print(t.contactUpper);
  Serial.print("/");
  Serial.print(t.upperSlightAway);
  Serial.print("/");
  Serial.print(t.upperMediumAway);
  Serial.print("/");
  Serial.println(t.upperFarAway);

  // Distanz mitte
  Serial.print(" middleDistance(Contact/Slight/Medium/Far)=");
  Serial.print(t.contactMiddle);
  Serial.print("/");
  Serial.print(t.middleSlightAway);
  Serial.print("/");
  Serial.print(t.middleMediumAway);
  Serial.print("/");
  Serial.println(t.middleFarAway);

  // Distanz unten
  Serial.print(" lowerDistance(Slight/Medium/Far)=");
  Serial.print(t.contactLower);
  Serial.print("/");
  Serial.print(t.lowerSlightAway);
  Serial.print("/");
  Serial.print(t.lowerMediumAway);
  Serial.print("/");
  Serial.println(t.lowerFarAway);

  // Torso global
  Serial.print(" | torsoForward=");
  Serial.print(t.torsoForward);
  Serial.print(" torsoBackward=");
  Serial.println(t.torsoBackward);

  // Asymmetrien
  Serial.print(" | upperAsymmetry=");
  Serial.print(t.upperAsymmetry);
  Serial.print(" middleAsymmetry=");
  Serial.print(t.middleAsymmetry);
  Serial.print(" lowerAsymmetry=");
  Serial.println(t.lowerAsymmetry);

  // Drehrichtung / Twist
  Serial.print(" | twistUpper(L/R)=");
  Serial.print(t.upperTwistLeft);
  Serial.print("/");
  Serial.print(t.upperTwistRight);

  Serial.print(" twistMiddle(L/R)=");
  Serial.print(t.middleTwistLeft);
  Serial.print("/");
  Serial.print(t.middleTwistRight);

  Serial.print(" twistLower(L/R)=");
  Serial.print(t.lowerTwistLeft);
  Serial.print("/");
  Serial.print(t.lowerTwistRight);

  Serial.println();
}

void loop() {
  matrix.update(state);
  fsr.update(state);
  tofm.update(state);

  baselineMgr.update();

  if (Serial.available()) {
    if (Serial.read() == 'b') {
      baselineMgr.start(15000, true, true);
      Serial.println("start baseline");
    }
  }

  if (baselineMgr.baselineJustFinished()) {
    Serial.println(F("[BASELINE] Matrix baseline finished. Dump:"));
    auto &mp = baselineMgr.matrixPosture();
    const float *b = mp.getBaseline();
    uint16_t n = mp.getSize();

    Serial.print("[");
    for (uint16_t i = 0; i < n; ++i) {
      if (i)
        Serial.print(",");
      Serial.print(b[i], 1);
    }
    Serial.println("]");

    // NEU: ToF-Baseline dumpen (falls vorhanden)
    ToFPosture *tp = baselineMgr.tofPosture();
    if (tp && baselineMgr.hasTofBaseline()) {
      const float *backBase = tp->getBackBaseline();
      const float *headBase = tp->getHeadBaseline();
      const float *kneeBase = tp->getKneeBaseline();
      const float *bottomBase = tp->getBottomBaseline();

      Serial.println(F("[BASELINE] ToF baseline (back):"));
      Serial.print("[");
      for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
        if (i)
          Serial.print(",");
        Serial.print(backBase[i], 1);
      }
      Serial.println("]");

      Serial.println(F("[BASELINE] ToF baseline (head):"));
      Serial.print("[");
      for (uint8_t i = 0; i < TofData::HEAD_N; ++i) {
        if (i)
          Serial.print(",");
        Serial.print(headBase[i], 1);
      }
      Serial.println("]");

      Serial.println(F("[BASELINE] ToF baseline (knee):"));
      Serial.print("[");
      for (uint8_t i = 0; i < TofData::KNEE_N; ++i) {
        if (i)
          Serial.print(",");
        Serial.print(kneeBase[i], 1);
      }
      Serial.println("]");

      Serial.println(F("[BASELINE] ToF baseline (bottom):"));
      Serial.print("[");
      for (uint8_t i = 0; i < TofData::BOTTOM_N; ++i) {
        if (i)
          Serial.print(",");
        Serial.print(bottomBase[i], 1);
      }
      Serial.println("]");
    }
  }

  state.t_ms = millis();
  if (baselineMgr.isRunning()) {
    static unsigned long lastInfo = 0;
    if (millis() - lastInfo > 500) { // z.B. alle 500 ms
      lastInfo = millis();
      Serial.print(F("[BASELINE] running, remaining ms = "));
      Serial.println(baselineMgr.remainingMs());
    }
  } else {
    // print_json(state, Serial);
    // Serial.println();
  }
  // MatrixTags tagsOld = matrixPosture.computeTags();
  MatrixTags tagsNew = matrixPosture.computeTagsBaseline();

  // Serial.print("OLD: occ=");
  // Serial.print(tagsOld.occupied);
  // Serial.print(" L=");
  // Serial.print(tagsOld.weightLeft);
  // Serial.print(" R=");
  // Serial.print(tagsOld.weightRight);
  // Serial.print(" F=");
  // Serial.print(tagsOld.weightForward);
  // Serial.print(" B=");
  // Serial.print(tagsOld.weightBackward);
  // Serial.print(" N=");
  // Serial.print(tagsOld.neutral);
  Serial.println();
  Serial.print("NEW: occ=");
  Serial.print(tagsNew.occupied);
  Serial.print(" L=");
  Serial.print(tagsNew.weightLeft);
  Serial.print(" R=");
  Serial.print(tagsNew.weightRight);
  Serial.print(" F=");
  Serial.print(tagsNew.weightForward);
  Serial.print(" B=");
  Serial.print(tagsNew.weightBackward);
  Serial.print(" N=");
  Serial.println(tagsNew.neutral);

  ToFTags ttags = tofPosture.computeTags();
  debugPrintToFTags(ttags);

  Serial.println("------");
  delay(500);
}