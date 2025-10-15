#pragma once
#include <Arduino.h>
#include "telemetry.h"
#include "posture_labels.h"

// helper: print generic array
template<typename T>
inline void print_array(Stream& out, const T* data, size_t n) {
  out.print("[");
  for (size_t i = 0; i < n; ++i) {
    if (i) out.print(",");
    out.print(data[i]);
  }
  out.print("]");
}

//out matrix flat
inline void print_matrix_json(Stream& out, const MatrixData& m) {
  out.print("{\"rows\":");
  out.print((int)MatrixData::ROWS);
  out.print(",\"cols\":");
  out.print((int)MatrixData::COLS);
  out.print(",\"values\":[");
  const int total = MatrixData::ROWS * MatrixData::COLS;
  for (int i = 0; i < total; ++i) {
    if (i) out.print(",");
    out.print(m.values[i]);
  }
  out.print("]}");
}

// FSR as values
inline void print_fsr_json(Stream& out, const FsrData& fsr) {
  out.print("{\"values\":");
  print_array(out, fsr.values, FSR_COUNT);
  out.print("}");
}

// ToF in zones
inline void print_tof_json(Stream& out, const TofData& t) {
  out.print("{\"back\":");
  print_array(out, t.back,   TofData::BACK_N);
  out.print(",\"head\":");
  print_array(out, t.head,   TofData::HEAD_N);
  out.print(",\"knee\":");
  print_array(out, t.knee,   TofData::KNEE_N);
  out.print(",\"bottom\":");
  print_array(out, t.bottom, TofData::BOTTOM_N);
  out.print("}");
}

// Main packaging
inline void print_json(const Telemetry& t, Stream& out) {
  out.print("{\"schema\":");       out.print(t.schema);
  out.print(",\"timestamp_ms\":"); out.print(t.t_ms);

  out.print(",\"posId\":");        out.print((int)t.posId);
  out.print(",\"posLabel\":\"");    out.print(posIdToString(t.posId)); out.print("\"");

  out.print(",\"data\":{");

  out.print("\"matrix\":");
  print_matrix_json(out, t.matrix);
  out.print(",\"fsr\":");
  print_fsr_json(out, t.fsr);
  out.print(",\"tof\":");
  print_tof_json(out, t.tof);

  out.print("}"); // data

//   out.print(",\"meta\":{");
//   out.print("\"fsr_map\":{\"0\":\"lordosis\",\"1\":\"left_arm\",\"2\":\"right_arm\"}");
//   out.print(",\"posId_map\":{\"0\":\"neutral\",\"1\":\"lean_left\",\"2\":\"lean_right\",\"3\":\"slouch\",\"4\":\"perch\"}");
//   out.print("}");

  out.print("}"); // root
}

// toString
inline String to_json_string(const Telemetry& t) {
  String s; s.reserve(256);
  class StringStream : public Stream {
    String& s;
  public:
    explicit StringStream(String& s): s(s) {}
    int available() override { return 0; }
    int read() override { return -1; }
    int peek() override { return -1; }
    void flush() override {}
    size_t write(uint8_t c) override { s += char(c); return 1; }
    size_t write(const uint8_t* buf, size_t sz) override { s.concat((const char*)buf, sz); return sz; }
  } ss(s);
  print_json(t, ss);
  return s;
}