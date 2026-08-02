#include "core/posture/matrix/matrix_posture.h"
#include "core/posture/matrix/matrix_zones.h"
#include "core/posture/posture_classifier.h"
#include "core/posture/tof/back_meta.h"
#include "core/posture/tof/tof_posture.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

int assertions = 0;
int failures = 0;

void check(bool condition, const std::string &message) {
  ++assertions;
  if (!condition) {
    ++failures;
    std::cerr << "FAIL: " << message << '\n';
  }
}

void checkNear(float actual, float expected, float tolerance,
               const std::string &message) {
  check(std::fabs(actual - expected) <= tolerance,
        message + " (expected " + std::to_string(expected) + ", got " +
            std::to_string(actual) + ")");
}

void checkPosture(const PostureClassifier &classifier, const MatrixTags &matrix,
                  const BackMetaTags &back, PosId expected,
                  const std::string &message) {
  check(classifier.classify(matrix, back) == expected, message);
}

void testMatrixZones() {
  MatrixZones zones(7, 7);

  check(zones.zoneCount() == 8, "matrix exposes eight zones");
  check(zones.zone(MatrixZoneId::LeftHalf).count == 28,
        "left half includes center column");
  check(zones.zone(MatrixZoneId::RightHalf).count == 28,
        "right half includes center column");
  check(zones.zone(MatrixZoneId::FrontHalf).count == 28,
        "front half includes center row");
  check(zones.zone(MatrixZoneId::BackHalf).count == 28,
        "back half includes center row");
  check(zones.zone(MatrixZoneId::FrontLeft).count == 9,
        "front-left quadrant excludes center axes");
  check(zones.zone(MatrixZoneId::BackRight).count == 9,
        "back-right quadrant excludes center axes");
}

void testMatrixBaselineAndTags() {
  int16_t values[49];
  std::fill_n(values, 49, int16_t{100});
  MatrixPosture posture(values, 7, 7);

  posture.feedBaselineSample();
  std::fill_n(values, 49, int16_t{200});
  values[0] = MatrixPosture::INVALID_INT;
  posture.feedBaselineSample();
  posture.finalizeBaseline();

  check(posture.hasBaseline(), "matrix baseline finalizes");
  check(posture.getFrameCount() == 2, "matrix baseline counts frames");
  checkNear(posture.getBaseline()[0], 100.0f, 0.01f,
            "invalid readings are ignored in baseline mean");
  checkNear(posture.getBaseline()[1], 150.0f, 0.01f,
            "valid readings are averaged in baseline mean");

  std::fill_n(values, 49, int16_t{150});
  MatrixTags neutral = posture.computeTagsBaseline();
  check(neutral.occupied, "baseline load is recognized as occupied");
  check(neutral.neutral, "balanced baseline load is neutral");
  check(!neutral.weightLeft && !neutral.weightRight,
        "balanced load has no lateral tag");

  std::fill_n(values, 49, int16_t{0});
  MatrixTags empty = posture.computeTagsBaseline();
  check(!empty.occupied, "low seat load is unoccupied");

  std::fill_n(values, 49, int16_t{150});
  for (int row = 0; row < 7; ++row) {
    for (int col = 4; col < 7; ++col) {
      values[row * 7 + col] = 600;
    }
  }
  MatrixTags left = posture.computeTagsBaseline();
  check(left.occupied, "lateral load remains occupied");
  check(left.weightLeft, "physical left-side load produces left tag");
  check(!left.neutral, "strong lateral load is not neutral");
}

void testTofBaselineAndTags() {
  TofData data;
  std::fill_n(data.back, TofData::BACK_N, int32_t{100});
  ToFPosture posture(&data);

  posture.feedBaselineSample();
  std::fill_n(data.back, TofData::BACK_N, int32_t{120});
  data.back[0] = ToFPosture::INVALID;
  posture.feedBaselineSample();
  posture.finalizeBaseline();

  check(posture.hasBaseline(), "ToF baseline finalizes");
  check(posture.getFrameCount() == 2, "ToF baseline counts frames");
  checkNear(posture.getBackBaseline()[0], 100.0f, 0.01f,
            "invalid ToF reading is ignored in baseline mean");
  checkNear(posture.getBackBaseline()[1], 110.0f, 0.01f,
            "valid ToF readings are averaged");

  for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
    data.back[i] = static_cast<int32_t>(posture.getBackBaseline()[i]);
  }
  ToFTags neutral = posture.computeTags();
  check(neutral.backInRange, "valid back sensors establish presence");
  check(neutral.contact, "baseline-relative back position has full contact");
  check(neutral.neutral, "symmetric baseline-relative back is neutral");
  check(!neutral.torsoForward && !neutral.torsoBackward,
        "baseline has no torso direction");

  for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
    data.back[i] = static_cast<int32_t>(posture.getBackBaseline()[i] + 50);
  }
  ToFTags forward = posture.computeTags();
  check(forward.backInRange, "forward torso remains within sensing range");
  check(forward.torsoForward, "positive mean ToF delta is torso forward");

  std::fill_n(data.back, TofData::BACK_N, ToFPosture::INVALID);
  ToFTags missing = posture.computeTags();
  check(!missing.backInRange, "missing ToF readings do not establish presence");
}

void testBackMeta() {
  check(!computeBackMeta(ToFTags{}).inRange,
        "out-of-range ToF data yields empty back meta");

  ToFTags slouch;
  slouch.backInRange = true;
  slouch.upperMediumAway = true;
  slouch.middleSlightAway = true;
  slouch.contactLower = true;
  BackMetaTags slouchMeta = computeBackMeta(slouch);
  check(slouchMeta.slouchShoulder,
        "upper zone farther than middle yields shoulder slouch");
  check(slouchMeta.slouchTorso,
        "middle zone farther than lower yields torso slouch");
  check(slouchMeta.hyperlordosis,
        "lower contact plus upper/middle distance yields hyperlordosis tag");

  ToFTags twist;
  twist.backInRange = true;
  twist.upperTwistLeft = true;
  BackMetaTags partialTwist = computeBackMeta(twist);
  check(partialTwist.leftTwist && !partialTwist.leftTwistFull,
        "upper-only asymmetry yields partial left twist");

  twist.lowerTwistLeft = true;
  BackMetaTags fullTwist = computeBackMeta(twist);
  check(fullTwist.leftTwistFull && !fullTwist.leftTwist,
        "upper and lower asymmetry yield full left twist");
}

void testClassifier() {
  PostureClassifier classifier;
  MatrixTags matrix;
  BackMetaTags back;

  checkPosture(classifier, matrix, back, PosId::UNOCCUPIED,
               "unoccupied has highest-level early return");

  matrix.occupied = true;
  matrix.neutral = true;
  back.backNeutral = true;
  back.backNear = true;
  checkPosture(classifier, matrix, back, PosId::TRUE_NEUTRAL,
               "neutral matrix and back classify true neutral");

  back = {};
  checkPosture(classifier, matrix, back, PosId::NEUTRAL_NO_BACKREST,
               "neutral without back contact classifies no-backrest");

  matrix = {};
  matrix.occupied = true;
  matrix.weightLeft = true;
  matrix.neutral = true;
  checkPosture(classifier, matrix, back, PosId::LEAN_NEUTRAL_LEFT,
               "mild left lean classification");
  matrix.neutral = false;
  checkPosture(classifier, matrix, back, PosId::LEAN_STRONG_LEFT,
               "strong left lean classification");

  matrix = {};
  matrix.occupied = true;
  matrix.weightRight = true;
  matrix.neutral = true;
  checkPosture(classifier, matrix, back, PosId::LEAN_NEUTRAL_RIGHT,
               "mild right lean classification");
  matrix.neutral = false;
  checkPosture(classifier, matrix, back, PosId::LEAN_STRONG_RIGHT,
               "strong right lean classification");

  matrix = {};
  matrix.occupied = true;
  matrix.weightForward = true;
  checkPosture(classifier, matrix, back, PosId::SIT_FRONT,
               "forward load without back contact classifies sit-front");

  matrix = {};
  matrix.occupied = true;
  matrix.weightBackward = true;
  back.backNear = true;
  checkPosture(classifier, matrix, back, PosId::LEAN_BACK,
               "backward load with contact classifies lean-back");

  matrix = {};
  matrix.occupied = true;
  matrix.neutral = true;
  back = {};
  back.slouchShoulder = true;
  checkPosture(classifier, matrix, back, PosId::SLOUCH_UPPER_NEUTRAL,
               "shoulder slouch classifies upper-neutral slouch");
  back.slouchTorso = true;
  checkPosture(classifier, matrix, back, PosId::SLOUCH_LOWER_NEUTRAL,
               "combined slouch classifies lower-neutral slouch");

  matrix.weightLeft = true;
  checkPosture(classifier, matrix, back, PosId::SLOUCH_LOWER_LEFT,
               "combined slouch with left load classifies lower-left");
  back.slouchTorso = false;
  checkPosture(classifier, matrix, back, PosId::SLOUCH_UPPER_LEFT,
               "shoulder slouch with left load classifies upper-left");
  matrix.weightLeft = false;
  matrix.weightRight = true;
  checkPosture(classifier, matrix, back, PosId::SLOUCH_UPPER_RIGHT,
               "shoulder slouch with right load classifies upper-right");
  back.slouchTorso = true;
  checkPosture(classifier, matrix, back, PosId::SLOUCH_LOWER_RIGHT,
               "combined slouch with right load classifies lower-right");

  matrix = {};
  matrix.occupied = true;
  back = {};
  checkPosture(classifier, matrix, back, PosId::UNKNOWN,
               "unmatched occupied state remains unknown");
}

} // namespace

int main() {
  testMatrixZones();
  testMatrixBaselineAndTags();
  testTofBaselineAndTags();
  testBackMeta();
  testClassifier();

  if (failures == 0) {
    std::cout << "PASS: " << assertions << " assertions\n";
    return 0;
  }

  std::cerr << "FAILED: " << failures << " of " << assertions
            << " assertions\n";
  return 1;
}
