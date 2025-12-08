#pragma once
#include <stdint.h>

// Meta-Beschreibung der Rückenhaltung, aufgebaut auf ToFTags/BackZoneLevels
struct BackMeta {
  // grobe Distanz zur Lehne (exklusiv; genau eins sollte true sein)
  bool backNear = false;
  bool backMid = false;
  bool backFar = false;

  // Slouch-Typen
  bool slouchUpper = false; // Schultern / oberer Rücken nach vorne
  bool slouchMid = false;   // C-Slouch, Mitte rund
  bool slouchDeep = false;  // stark zusammengesackt

  // „Hohlkreuz“-ähnliches Muster
  bool lordosisLike = false;

  // tief angelehnt, v. a. oberer Rücken an Lehne
  bool reclineHigh = false;

  // globale Twist-Metas (was du wolltest):
  bool leftTwist = false;     // leichte Drehung nach links
  bool leftTwistFull = false; // deutliche Drehung (inkl. unterem Rücken)
  bool rightTwist = false;
  bool rightTwistFull = false;

  // Neutral-Übernahme für den Rücken (aus ToFTags.neutral)
  bool backNeutral = false;
};