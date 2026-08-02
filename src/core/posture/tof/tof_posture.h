#pragma once
#include "core/tof_data.h"
#include <Arduino.h>
#include <stdint.h>

// -------------------- Enums für Levels --------------------

// Abstand zur Lehne relativ zur Baseline
enum class DistanceLevel : uint8_t {
  Unknown = 0,
  Contact,    // etwa Baseline (oder leicht näher)
  SlightAway, // moderat weiter weg
  MediumAway, // deutlich weiter weg
  FarAway     // sehr weit weg
};

// Stärke der Asymmetrie (links vs. rechts)
enum class AsymLevel : uint8_t { None = 0, Mild, Strong };

enum class BackRow : uint8_t { Upper, Middle, Lower };
// -------------------- Back-Zonen-Levelstruktur --------------------

struct BackZoneLevels {
  bool valid = false; // ob überhaupt sinnvoll berechnet

  // 3 vertikale Zonen
  DistanceLevel upper = DistanceLevel::Unknown;
  DistanceLevel middle = DistanceLevel::Unknown;
  DistanceLevel lower = DistanceLevel::Unknown;

  AsymLevel upperAsym = AsymLevel::None;
  AsymLevel middleAsym = AsymLevel::None;
  AsymLevel lowerAsym = AsymLevel::None;

  bool upperTwistLeft = false;
  bool upperTwistRight = false;

  bool middleTwistLeft = false;
  bool middleTwistRight = false;

  bool lowerTwistLeft = false;
  bool lowerTwistRight = false;
};

// -------------------- ToF-Tags --------------------

// Grobe ToF-basierte Haltungshinweise
// -------------------- ToF-Tags --------------------

// Grobe ToF-basierte Haltungshinweise
struct ToFTags {
  // Präsenz / Grundzustand
  bool backInRange = false; // Sensors see "something" in range ~50cm
  bool neutral = false;     // Rücken relativ gerade (oben/mitte/unten ähnliche
                            // Distanz, wenig Asymmetrie)

  // Kontakt zur Lehne
  bool contact = false; // irgendeine Zone in Kontakt
  bool contactUpper = false;
  bool contactMiddle = false;
  bool contactLower = false;

  // Distanzkategorien (nur Rücken, vertikal)
  bool upperSlightAway = false;
  bool upperMediumAway = false;
  bool upperFarAway = false;

  bool middleSlightAway = false;
  bool middleMediumAway = false;
  bool middleFarAway = false;

  bool lowerSlightAway = false;
  bool lowerMediumAway = false;
  bool lowerFarAway = false;

  // Globale Torso-Neigung relativ zur Baseline
  bool torsoForward = false;  // gesamter Rücken weiter weg als Baseline
  bool torsoBackward = false; // gesamter Rücken näher an Lehne als Baseline

  // Asymmetrien / Drehungen
  bool upperAsymmetry = false;
  bool middleAsymmetry = false;
  bool lowerAsymmetry = false;

  bool upperTwistLeft = false;
  bool upperTwistRight = false;
  bool middleTwistLeft = false;
  bool middleTwistRight = false;
  bool lowerTwistLeft = false;
  bool lowerTwistRight = false;
};

// -------------------- ToFPosture-Klasse --------------------

class ToFPosture {
public:
  static constexpr int32_t INVALID = -1;

  explicit ToFPosture(const TofData *tofPtr);

  // Baseline-Lebenszyklus
  void beginBaseline();
  void feedBaselineSample();
  void finalizeBaseline();

  bool hasBaseline() const { return _hasBaseline; }
  uint16_t getFrameCount() const { return _frames; }

  // Zugriff auf Baselines (für Debug-Dumps)
  const float *getBackBaseline() const { return _backBaseline; }
  const float *getHeadBaseline() const { return _headBaseline; }
  const float *getKneeBaseline() const { return _kneeBaseline; }
  const float *getBottomBaseline() const { return _bottomBaseline; }

  // Aggregate-Deltas (optional)
  float backDeltaMean() const;
  float kneeDeltaMean() const;
  float bottomDeltaMean() const;

  // Levels für den Rücken (oben/mitte/unten, inkl. Asymmetrie)
  BackZoneLevels computeBackLevels() const;

  // Hauptfunktion: ToF-Tags aus aktuellen Messwerten + Baseline
  ToFTags computeTags() const;

private:
  const TofData *_tof = nullptr;
  bool _hasBaseline = false;
  uint16_t _frames = 0;

  // je Sensorgruppe: gleitender Mittelwert + Sample-Count
  float _backBaseline[TofData::BACK_N];
  uint16_t _backCount[TofData::BACK_N];

  float _headBaseline[TofData::HEAD_N];
  uint16_t _headCount[TofData::HEAD_N];

  float _kneeBaseline[TofData::KNEE_N];
  uint16_t _kneeCount[TofData::KNEE_N];

  float _bottomBaseline[TofData::BOTTOM_N];
  uint16_t _bottomCount[TofData::BOTTOM_N];

  // Hilfsfunktionen für pro-Sensor-Delta
  float deltaBackAt(uint8_t idx) const;
  float deltaHeadAt(uint8_t idx) const;
  float deltaKneeAt(uint8_t idx) const;
  float deltaBottomAt(uint8_t idx) const;

  // Mapping Delta → DistanceLevel
  DistanceLevel mapDeltaToLevel(float d, BackRow row) const;

  // Mapping Delta-Differenz links/rechts → AsymLevel
  AsymLevel classifyAsym(float diff) const;

  // Hilfsfunktion für Neutral-Bedingung
  int distanceRank(DistanceLevel d) const;
};