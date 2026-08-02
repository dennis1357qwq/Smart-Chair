#pragma once
#include <Arduino.h>

#include "core/posture/matrix/matrix_posture.h" // MatrixTags
#include "core/posture/tof/back_meta_tags.h"    // BackMetaTags
#include "core/posture/tof/tof_posture.h"       // ToFTags
#include "core/posture_ids.h"                   // PosId
#include "core/telemetry.h"                     // Telemetry

namespace Debug {

// single print (Overloads)
void print(const MatrixTags &m);
void print(const ToFTags &t);
void print(const BackMetaTags &b);
void print(PosId id);
void print(const Telemetry &s);

// convenience print - all at once
void print(const MatrixTags &m, const ToFTags &t, const BackMetaTags &b,
           PosId id);

} // namespace Debug