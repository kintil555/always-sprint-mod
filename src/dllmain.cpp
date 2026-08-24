#pragma once
#define NOMINMAX

#include "amethyst/runtime/events/InputEvents.hpp"
#include "amethyst/runtime/events/ModEvents.hpp"
#include "amethyst/runtime/events/UiEvents.hpp"
#include "amethyst/runtime/events/GameEvents.hpp"
#include "amethyst/runtime/ModContext.hpp"
#include "amethyst/Log.hpp"

#include "dllmain.hpp"

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
// Toggled from the pause-menu button, and read every tick to decide whether
// we should force the vanilla "sprint" input to stay held down.
static bool g_alwaysSprintEnabled = false;

// ---------------------------------------------------------------------------
// Pause menu button handler
// ---------------------------------------------------------------------------
// NOTE: `ScreenEvent`'s member layout is not part of the public AmethystAPI
// runtime headers (unlike EventBus/InputManager, which are). Replay.dll
// resolves this the same way: it matches the pressed button's control name
// against a hardcoded string ("button.replay_toggle_record") inside its own
// UIButtonHandleEvent handler. You'll need to confirm the exact accessor
// (commonly something like `event.mScreenEvent.getControlName()` or a public
// `mControlName` field) against whatever ScreenEvent header your Amethyst_src
// checkout / IDA dump exposes -- the field name below is a placeholder.
void OnUIButtonHandle(UIButtonHandleEvent& event) {
    // if (event.mScreenEvent.mControlName != "button.always_sprint_toggle") return;
    // g_alwaysSprintEnabled = !g_alwaysSprintEnabled;
    // Log::Info("[AlwaysSprint] Toggled -> {}", g_alwaysSprintEnabled);
}

// ---------------------------------------------------------------------------
// Input hijack: keep the vanilla "sprint" action permanently "pressed"
// ---------------------------------------------------------------------------
// We don't fabricate our own keybind; instead we hook into the *existing*
// vanilla sprint input so double-tap-to-sprint / sprint-key logic upstream
// keeps working normally when the toggle is off, and simply never lets go
// when it's on.
void RegisterInputs(RegisterInputsEvent& event) {
    Amethyst::InputAction& sprintInput = event.inputManager.GetVanillaInput("sprint");

    sprintInput.addButtonUpHandler([](FocusImpact focus, ClientInstance& client) {
        if (g_alwaysSprintEnabled) {
            // Swallow the release entirely -> game keeps thinking sprint is held.
            return Amethyst::InputPassthrough::Consume;
        }
        return Amethyst::InputPassthrough::Passthrough;
    });
}

// ---------------------------------------------------------------------------
// Per-tick safety net
// ---------------------------------------------------------------------------
// Some server/singleplayer sprint state (Actor::setSprinting / player sprint
// flag) can still get cleared by things unrelated to the input handler
// (hunger, sneaking, etc). If you find always-sprint drops out in specific
// cases, this is the tick you'd extend with a direct call into the local
// player's sprint flag -- but that requires the LocalPlayer/Player class
// layout, which (like ScreenEvent) isn't in the public runtime headers.
// The input-hijack above is deliberately the primary mechanism because it
// only depends on documented, public Amethyst API surface.
void OnAfterTick(AfterTickEvent& event) {
    // Intentionally left minimal.
}

// ---------------------------------------------------------------------------
// Mod entrypoint
// ---------------------------------------------------------------------------
ModFunction void Initialize(AmethystContext& ctx, const Amethyst::Mod& mod) {
    Amethyst::InitializeAmethystMod(ctx, mod);

    auto& events = Amethyst::GetEventBus();
    events.AddListener<RegisterInputsEvent>(RegisterInputs);
    events.AddListener<UIButtonHandleEvent>(OnUIButtonHandle);
    events.AddListener<AfterTickEvent>(OnAfterTick);

    Log::Info("[AlwaysSprint] Mod successfully initialized!");
}
