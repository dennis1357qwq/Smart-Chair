#include "http_server.h"

#include "core/telemetry_json.h"
#include <StreamString.h>

static void addCorsHeaders(WebServer &server) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

HttpServer::HttpServer(Telemetry &state, BaselineManager &baselineMgr)
    : _state(state), _baselineMgr(baselineMgr), _server(80) {}

void HttpServer::begin(const char *ssid, const char *pass) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("[WiFi] connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());

  _server.on("/", HTTP_GET, [this]() {
    addCorsHeaders(_server);
    handleRoot();
  });
  _server.on("/telemetry", HTTP_GET, [this]() {
    addCorsHeaders(_server);
    handleTelemetry();
  });
  _server.on("/baseline/start", HTTP_GET, [this]() {
    addCorsHeaders(_server);
    handleBaselineStart();
  });
  _server.on("/baseline/start", HTTP_POST, [this]() {
    addCorsHeaders(_server);
    handleBaselineStart();
  });
  _server.on("/baseline/status", HTTP_GET, [this]() {
    addCorsHeaders(_server);
    handleBaselineStatus();
  });

  _server.begin();
  _ready = true;
  Serial.println("[HTTP] server started");
}

void HttpServer::update() {
  if (!_ready)
    return;
  _server.handleClient();
}

void HttpServer::handleRoot() { _server.send(200, "text/plain", "ok"); }

int HttpServer::getIntArg(const char *name, int fallback) {
  if (!_server.hasArg(name))
    return fallback;
  return _server.arg(name).toInt();
}

void HttpServer::handleTelemetry() {
  StreamString ss;
  ss.reserve(4096);
  print_json(_state, ss);

  _server.send(200, "application/json", ss);
}

void HttpServer::handleBaselineStart() {
  int ms = getIntArg("ms", 15000);
  bool doMatrix = getIntArg("matrix", 1) != 0;
  bool doTof = getIntArg("tof", 1) != 0;

  _baselineMgr.start((uint32_t)ms, doMatrix, doTof);

  StreamString ss;
  ss.print("{\"ok\":true,\"started\":true,\"ms\":");
  ss.print(ms);
  ss.print(",\"matrix\":");
  ss.print(doMatrix ? "true" : "false");
  ss.print(",\"tof\":");
  ss.print(doTof ? "true" : "false");
  ss.print("}");

  _server.send(202, "application/json", String(ss.c_str()));
}

void HttpServer::handleBaselineStatus() {
  bool running = _baselineMgr.isRunning();
  uint32_t rem = _baselineMgr.remainingMs();

  StreamString ss;
  ss.print("{\"running\":");
  ss.print(running ? "true" : "false");
  ss.print(",\"remainingMs\":");
  ss.print(rem);
  ss.print("}");

  _server.send(200, "application/json", String(ss.c_str()));
}