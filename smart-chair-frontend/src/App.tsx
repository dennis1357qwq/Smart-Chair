import { Outlet } from "react-router-dom";
import NavBar from "./components/NavBar";
import Sidebar from "./components/Sidebar";
import GlobalTimerWidget from "./components/GlobalTimerWidget";

function App() {
  return (
    <>
      <div className="flex">
        <Sidebar />
        <div className="flex-1 relative">
          <NavBar />
          <div className="overflow-y-auto h-[calc(100vh-4rem)]">
            <Outlet />
            <GlobalTimerWidget />
          </div>
        </div>
      </div>
    </>
  );
}

export default App;
