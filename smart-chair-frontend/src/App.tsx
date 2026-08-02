import { Outlet } from "react-router-dom";
import NavBar from "./ui/NavBar";
import Sidebar from "./ui/Sidebar";
import GlobalMiniWidget from "./ui/GlobalMiniWidget";

function App() {
  return (
    <div className="flex">
      <Sidebar />
      <div className="flex-1 relative">
        <NavBar />
        <div className="overflow-y-auto h-[calc(100vh-4rem)]">
          <Outlet />
          <GlobalMiniWidget />
        </div>
      </div>
    </div>
  );
}

export default App;
