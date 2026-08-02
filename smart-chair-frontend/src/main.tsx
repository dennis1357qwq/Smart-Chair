import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import App from "./App.tsx";
import { createBrowserRouter, RouterProvider } from "react-router-dom";
import "../index.css";
import Sessions from "./features/sessions/SessionsPage.tsx";
import SessionDetailPage from "./features/sessions/SessionDetailPage.tsx";
import { TimerProvider } from "./features/sessions/TimerContext.tsx";
import SettingsPage from "./features/settings/SettingsPage.tsx";
import { ChairProvider } from "./core/state/ChairContext.tsx";
import ChairPage from "./features/chair/ChairPage.tsx";
import ConfigPage from "./features/calibration/ConfigPage.tsx";

const router = createBrowserRouter([
  {
    path: "/",
    element: <App />,
    children: [
      { index: true, element: <ChairPage /> },
      { path: "sessions", element: <Sessions /> },
      { path: "sessions/:id", element: <SessionDetailPage /> },
      { path: "config", element: <ConfigPage /> },
      { path: "settings", element: <SettingsPage /> },
    ],
  },
]);

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <TimerProvider>
      <ChairProvider pollIntervalMs={800}>
        <RouterProvider router={router} />
      </ChairProvider>
    </TimerProvider>
  </StrictMode>
);
