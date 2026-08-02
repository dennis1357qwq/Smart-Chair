// vite.config.ts
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import tailwindcss from "@tailwindcss/vite";
import type { IncomingMessage, ServerResponse } from "node:http";
import {
  mockTelemetry,
  startMockBaseline,
  mockBaselineStatus,
} from "./src/dev/mockChair";

function sendJson(res: ServerResponse, status: number, body: unknown) {
  res.statusCode = status;
  res.setHeader("Content-Type", "application/json");
  res.setHeader("Access-Control-Allow-Origin", "*");
  res.setHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  res.setHeader("Access-Control-Allow-Headers", "Content-Type");
  res.end(JSON.stringify(body));
}

export default defineConfig({
  plugins: [
    react(),
    tailwindcss(),

    // --- mock chair API (dev only) ---
    {
      name: "mock-chair-api",
      configureServer(server) {
        server.middlewares.use(
          (req: IncomingMessage, res: ServerResponse, next) => {
            const url = req.url ?? "";

            if (req.method === "OPTIONS") {
              res.statusCode = 204;
              res.setHeader("Access-Control-Allow-Origin", "*");
              res.setHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
              res.setHeader("Access-Control-Allow-Headers", "Content-Type");
              res.end();
              return;
            }

            if (url.startsWith("/telemetry")) {
              return sendJson(res, 200, mockTelemetry());
            }

            if (url.startsWith("/baseline/start")) {
              const u = new URL("http://local" + url);
              const ms = Number(u.searchParams.get("ms") ?? "15000");
              startMockBaseline(Number.isFinite(ms) ? ms : 15000);
              return sendJson(res, 202, {
                ok: true,
                started: true,
                ms,
                matrix: true,
                tof: true,
              });
            }

            if (url.startsWith("/baseline/status")) {
              return sendJson(res, 200, mockBaselineStatus());
            }

            next();
          }
        );
      },
    },
  ],
});
