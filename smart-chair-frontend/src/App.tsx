import NavBar from "./components/NavBar";
import Sidebar from "./components/Sidebar";

function App() {
  return (
    <>
      <div className="flex">
        <Sidebar />
        <div className="flex-1 relative">
          <NavBar />
          <div className="overflow-y-auto h-[calc(100vh-4rem)]">Content</div>
        </div>
      </div>
    </>
  );
}

export default App;
