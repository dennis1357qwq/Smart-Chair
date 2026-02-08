#include "core/posture/tof/back_meta_tags.h"
#include "core/posture/tof/tof_posture.h"

static bool anyAway(bool slight, bool medium, bool far) {
  return slight || medium || far;
}

BackMetaTags computeBackMeta(const ToFTags &t) {
  BackMetaTags m;

  // ---- 1) Basic presence ----
  m.inRange = t.backInRange;
  m.backNeutral = t.neutral;

  if (!t.backInRange)
    return m;

  // ---- 2) Distance categories ----
  bool near = t.contactUpper || t.contactMiddle || t.contactLower ||
              t.contact || t.torsoBackward;
  bool far =
      t.upperFarAway || t.middleFarAway || t.lowerFarAway || t.torsoForward;

  m.backNear = near;
  m.backFar = (!near && far);
  m.backMid = (!near && !far);

  // ---- 3) Slouches ----
  // Upper slouch
  if ((t.upperMediumAway || t.upperFarAway) &&
      (t.contactMiddle || t.middleSlightAway)) {
    m.slouchUpper = true;
  }

  // Mid slouch
  if (t.middleMediumAway || t.middleFarAway) {
    m.slouchMid = true;
  }

  // Deep slouch
  if ((t.lowerMediumAway || t.lowerFarAway) &&
      (t.middleMediumAway || t.middleFarAway)) {
    m.slouchDeep = true;
  }

  // ---- 4) Hyperlordosis ----
  bool lowerNear = t.contactLower;
  bool upperAway =
      anyAway(t.upperSlightAway, t.upperMediumAway, t.upperFarAway);
  bool middleAway =
      anyAway(t.middleSlightAway, t.middleMediumAway, t.middleFarAway);

  if (lowerNear && (upperAway || middleAway)) {
    m.hyperlordosis = true;
  }

  // ---- 5) Recline high ----
  bool upperNear = t.contactUpper;
  bool lowerAway =
      anyAway(t.lowerSlightAway, t.lowerMediumAway, t.lowerFarAway);

  if (upperNear && lowerAway) {
    m.reclineHigh = true;
  }

  // ---- 6) Twists ----
  bool upperLeft = t.upperTwistLeft || t.middleTwistLeft;
  bool upperRight = t.upperTwistRight || t.middleTwistRight;

  bool lowerLeft = t.lowerTwistLeft;
  bool lowerRight = t.lowerTwistRight;

  if (upperLeft && !lowerLeft)
    m.leftTwist = true;
  if (upperRight && !lowerRight)
    m.rightTwist = true;

  if (upperLeft && lowerLeft)
    m.leftTwistFull = true;
  if (upperRight && lowerRight)
    m.rightTwistFull = true;

  return m;
}