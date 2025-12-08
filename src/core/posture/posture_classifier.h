#pragma once
#include "core/posture/back_meta_tags.h" // BackMeta
#include "core/posture/matrix_posture.h" // MatrixTags
#include "core/posture_ids.h"            // PosId

class PostureClassifier {
public:
  PosId classify(const MatrixTags &m, const BackMeta &b) const;
};