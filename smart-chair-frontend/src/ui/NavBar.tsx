import { Disclosure } from "@headlessui/react";

export default function NavBar() {
  return (
    <div className="">
      <Disclosure as="nav" className="top-0 left-0 right-0 bg-gray-800 h-16">
        <div className="mx-auto max-w-7xl px-4 sm:px-6 lg:px-8"></div>
      </Disclosure>
    </div>
  );
}
