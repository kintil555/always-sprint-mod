# AlwaysSprint

Amethyst mod for Minecraft Bedrock 1.21.0.3 — forces sprint to always stay on,
toggleable from the pause menu.

## How it works

- `src/dllmain.cpp` hooks the **vanilla** `sprint` input action via
  `InputManager::GetVanillaInput("sprint")` and swallows the button-up event
  when the toggle is enabled, so the game never sees sprint release.
- `data/packs/RP/ui/pause_screen.json` adds a toggle button next to the
  settings button in the pause menu, modeled on FrederoxDev's `Replay` mod UI.
- `OnUIButtonHandle` flips the toggle when the button is pressed.

**Not yet wired (see comments in dllmain.cpp):** the exact `ScreenEvent`
member used to read the pressed control's name isn't in the public
AmethystAPI runtime headers. Confirm the field/method against your
`Amethyst_src` checkout or a binary dump before this button will actually work.

## Setup (local dev build)

```powershell
git clone https://github.com/<you>/AlwaysSprint.git
cd AlwaysSprint

# Point AMETHYST_SRC at a local Amethyst checkout
$destination = Join-Path $env:USERPROFILE "Documents/Amethyst"
git clone --recurse-submodules https://github.com/FrederoxDev/Amethyst.git $destination
[System.Environment]::SetEnvironmentVariable("amethyst_src", $destination, "User")

# Build resources (requires deno + regolith)
cd data
regolith install-all
regolith run local
cd ..

# Build the DLL (requires xmake)
xmake f -y
xmake
```

Output lands in `dist/`. Copy `dist/` contents + `mod.json` into your
Amethyst mods folder as `AlwaysSprint@<version>`, or just use the zip
produced by CI (see below).

## CI / Releases

`.github/workflows/build.yml` is a manual (`workflow_dispatch`) pipeline that:
1. Bumps the patch version in `mod.json` via `VersionTools.ts` and pushes it.
2. Builds resources with regolith and the DLL with xmake.
3. Zips `dist/` and publishes it as a GitHub Release (`AlwaysSprint@x.y.z.zip`).

Trigger it from the **Actions** tab → **Publish Version** → **Run workflow**.
No extra secrets needed — it uses the default `GITHUB_TOKEN`.

## Requirements

- Windows 10+, Visual Studio (Desktop C++ workload), NASM on PATH
- [xmake](https://xmake.io/), [Deno](https://deno.com/), [Regolith](https://github.com/Bedrock-OSS/regolith)
- [Amethyst-Launcher](https://github.com/FrederoxDev/Amethyst-Launcher) to run the built mod in-game
