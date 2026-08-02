#include "debug_print.h"
#include "core/posture_labels.h" // posIdToString

namespace Debug {

void print(const MatrixTags &m) {
  Serial.print("[Matrix] occ=");
  Serial.print(m.occupied);
  Serial.print(" L=");
  Serial.print(m.weightLeft);
  Serial.print(" R=");
  Serial.print(m.weightRight);
  Serial.print(" F=");
  Serial.print(m.weightForward);
  Serial.print(" B=");
  Serial.print(m.weightBackward);
  Serial.print(" N=");
  Serial.println(m.neutral);
}

void print(const ToFTags &t) {
  Serial.print("[ToF] backInRange=");
  Serial.print(t.backInRange);
  Serial.print(" neutralBack=");
  Serial.print(t.neutral);

  Serial.print(" | contactFull=");
  Serial.println(t.contact);

  Serial.print(" | upper(C/S/M/F)=");
  Serial.print(t.contactUpper);
  Serial.print("/");
  Serial.print(t.upperSlightAway);
  Serial.print("/");
  Serial.print(t.upperMediumAway);
  Serial.print("/");
  Serial.println(t.upperFarAway);

  Serial.print(" | middle(C/S/M/F)=");
  Serial.print(t.contactMiddle);
  Serial.print("/");
  Serial.print(t.middleSlightAway);
  Serial.print("/");
  Serial.print(t.middleMediumAway);
  Serial.print("/");
  Serial.println(t.middleFarAway);

  Serial.print(" | lower(C/S/M/F)=");
  Serial.print(t.contactLower);
  Serial.print("/");
  Serial.print(t.lowerSlightAway);
  Serial.print("/");
  Serial.print(t.lowerMediumAway);
  Serial.print("/");
  Serial.println(t.lowerFarAway);

  Serial.print(" | torsoForward=");
  Serial.print(t.torsoForward);
  Serial.print(" torsoBackward=");
  Serial.println(t.torsoBackward);
}

void print(const BackMetaTags &b) {
  Serial.print("[BackMeta] inRange=");
  Serial.print(b.inRange);
  Serial.print(" neutral=");
  Serial.print(b.backNeutral);

  Serial.print(" | dist(N/M/F)=");
  Serial.print(b.backNear);
  Serial.print("/");
  Serial.print(b.backMid);
  Serial.print("/");
  Serial.print(b.backFar);

  Serial.print(" | slouch(T/S)=");
  Serial.print(b.slouchTorso);
  Serial.print("/");
  Serial.print(b.slouchShoulder);

  Serial.print(" | hyper=");
  Serial.print(b.hyperlordosis);
  Serial.print(" reclineHigh=");
  Serial.println(b.reclineHigh);
}

void print(PosId id) {
  Serial.print("[POS] ");
  Serial.print((int)id);
  Serial.print(" ");
  Serial.println(posIdToString(id));
}

void print(const Telemetry &s) {
  Serial.print("[State] t_ms=");
  Serial.print(s.t_ms);
  Serial.print(" schema=");
  Serial.print(s.schema);
  Serial.print(" posId=");
  Serial.print((int)s.posId);
  Serial.print(" label=");
  Serial.println(posIdToString(s.posId));
}

void print(const MatrixTags &m, const ToFTags &t, const BackMetaTags &b,
           PosId id) {
  print(m);
  print(b);
  print(id);
}

} // namespace Debug