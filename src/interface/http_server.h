#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#include "core/posture/baseline/baseline_manager.h"
#include "core/telemetry.h"

class HttpServer {
public:
  HttpServer(Telemetry &state, BaselineManager &baselineMgr);

  void begin(const char *ssid, const char *pass);
  void update();

  bool isReady() const { return _ready; }

private:
  Telemetry &_state;
  BaselineManager &_baselineMgr;

  WebServer _server;
  bool _ready = false;

  // handlers
  void handleRoot();
  void handleTelemetry();
  void handleBaselineStart();
  void handleBaselineStatus();

  // helper
  int getIntArg(const char *name, int fallback);
};