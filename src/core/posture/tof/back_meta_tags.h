#pragma once
#include <stdint.h>

// Meta-Beschreibung der Rückenhaltung, aufgebaut auf ToFTags/BackZoneLevels
struct BackMetaTags {
  // grobe Distanz zur Lehne (exklusiv; genau eins sollte true sein)
  bool backNear = false;
  bool backMid = false;
  bool backFar = false;

  // Slouch-Typen
  // bool slouchUpper = false;    // Schultern / oberer Rücken nach vorne
  // bool slouchMid = false;      // C-Slouch, Mitte rund
  // bool slouchDeep = false;     // stark zusammengesackt
  bool slouchShoulder = false; // “upper > middle”
  bool slouchTorso = false;    // "middle > lower"

  // „Hohlkreuz“-ähnliches Muster
  bool hyperlordosis = false;

  // tief angelehnt, v. a. oberer Rücken an Lehne
  bool reclineHigh = false;

  // globale Twist-Metas (was du wolltest):
  bool leftTwist = false;     // leichte Drehung nach links
  bool leftTwistFull = false; // deutliche Drehung (inkl. unterem Rücken)
  bool rightTwist = false;
  bool rightTwistFull = false;

  // Übernahme aus den Raw Tags
  bool backNeutral = false;
  bool inRange = false;
};