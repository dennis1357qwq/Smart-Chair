export default function ConfigPage() {
  const checks = [
    {
      title: "Seat height",
      items: [
        "Adjust height so your feet rest flat on the floor.",
        "Aim for knees at ~90° or slightly more open (>90°).",
        "Avoid pressure behind the knees; weight should be evenly distributed.",
      ],
    },
    {
      title: "Seat depth",
      items: [
        "Slide the seat so there is ~2–3 fingers (about 4–6 cm) of space between the seat edge and the back of your knees.",
        "If you feel pressure behind the knees, reduce seat depth.",
        "If you lack thigh support, increase seat depth slightly.",
      ],
    },
    {
      title: "Backrest & lumbar support",
      items: [
        "Sit back so your pelvis is supported and your lower back contacts the lumbar area.",
        "Adjust lumbar support to fit the natural curve of your lower back (not too high).",
        "If available, set backrest tension so you can lean back comfortably without collapsing.",
      ],
    },
    {
      title: "Armrests",
      items: [
        "Set armrests so shoulders stay relaxed (no shrugging).",
        "Elbows should rest at about 90° while typing/mousing.",
        "Keep forearms supported without pushing you away from the desk.",
      ],
    },
    {
      title: "Desk & screen",
      items: [
        "Keep wrists straight; raise/lower chair or desk to avoid bending wrists up.",
        "Top of the screen around eye level; distance about an arm’s length.",
        "Center the screen; avoid twisting your neck or torso.",
      ],
    },
    {
      title: "Quick posture check",
      items: [
        "Hips slightly higher than knees is usually fine.",
        "Ribs stacked over pelvis (avoid excessive arching or rounding).",
        "Take a short movement break every 30–60 minutes.",
      ],
    },
  ];

  return (
    <div className="p-6">
      <div className="max-w-3xl space-y-6">
        <header className="space-y-2">
          <h1 className="text-2xl font-semibold text-slate-900">
            Chair setup checklist
          </h1>
          <p className="text-slate-600">
            A quick step-by-step guide for a reasonable ergonomic baseline.
            Adjust to comfort — small changes matter.
          </p>
        </header>

        <div className="rounded-2xl border border-slate-200 bg-white p-5 shadow-sm">
          <div className="space-y-6">
            {checks.map((section) => (
              <div key={section.title}>
                <div className="text-sm font-semibold text-slate-900">
                  {section.title}
                </div>
                <ul className="mt-2 list-disc pl-5 space-y-1 text-sm text-slate-700">
                  {section.items.map((x) => (
                    <li key={x}>{x}</li>
                  ))}
                </ul>
              </div>
            ))}
          </div>
        </div>

        <div className="text-[12px] text-slate-500">
          Note: This is a lightweight guide for the prototype. It does not
          replace professional ergonomic assessment.
        </div>
      </div>
    </div>
  );
}
