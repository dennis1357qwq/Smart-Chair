import { useState } from "react";
import {
  HomeIcon,
  Cog6ToothIcon,
  ChartBarIcon,
  ChevronLeftIcon,
  ChevronRightIcon,
  WrenchIcon,
} from "@heroicons/react/24/outline";

const navigation = [
  { name: "Chair", icon: HomeIcon, route: "/" },
  { name: "Sessions", icon: ChartBarIcon, route: "/sessions" },
  { name: "Calibrate", icon: WrenchIcon, route: "/config" },
];

const settings = { name: "Settings", icon: Cog6ToothIcon, route: "/settings" };

export default function Sidebar() {
  const [collapsed, setCollapsed] = useState(false);

  return (
    <div
      className={`flex flex-col h-screen bg-gray-800 text-white transition-[width] duration-300 ${
        collapsed ? "w-16" : "w-45"
      }`}
    >
      <div className="flex items-center justify-between p-4 border-b border-gray-700">
        {!collapsed && (
          <span className="text-lg font-semibold">Smart Chair</span>
        )}
        <button
          onClick={() => setCollapsed((prev) => !prev)}
          className="p-1 text-gray-400 hover:text-white"
        >
          {collapsed ? (
            <ChevronRightIcon className="w-5 h-5" />
          ) : (
            <ChevronLeftIcon className="w-5 h-5" />
          )}
        </button>
      </div>

      <div className="flex flex-col justify-between flex-1">
        <nav className="flex flex-col p-2 space-y-1">
          {navigation.map((item) => (
            <SidebarItem
              key={item.name}
              name={item.name}
              icon={item.icon}
              route={item.route}
              collapsed={collapsed}
            />
          ))}
        </nav>

        <div className="p-2 border-t border-gray-700">
          <SidebarItem
            name={settings.name}
            icon={settings.icon}
            route={settings.route}
            collapsed={collapsed}
          />
        </div>
      </div>
    </div>
  );
}

function SidebarItem({ name, icon: Icon, route, collapsed }: any) {
  return (
    <a
      href={route}
      className="flex items-center space-x-3 rounded-md px-3 py-2 text-sm text-gray-300 hover:bg-gray-700"
    >
      <Icon className="w-5 h-5" />
      {!collapsed && <span>{name}</span>}
    </a>
  );
}
