import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import App from "./App.tsx";
import { createBrowserRouter, RouterProvider } from "react-router-dom";
import "../index.css";
import Sessions from "./components/Sessions.tsx";
import SessionDetailPage from "./components/SessionDetailPage.tsx";
import { TimerProvider } from "./components/TimerContext.tsx";

const router = createBrowserRouter([
  {
    path: "/",
    element: <App />,
    children: [
      { index: true, element: <></> },
      { path: "sessions", element: <Sessions /> },
      { path: "/sessions/:id", element: <SessionDetailPage /> },
      { path: "config", element: <>config</> },
    ],
  },
]);

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <TimerProvider>
      <RouterProvider router={router} />
    </TimerProvider>
  </StrictMode>
);
