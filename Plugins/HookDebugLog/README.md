# HookDebugLog

A lightweight Unreal Engine debug logging plugin that provides color-coded on-screen messages, output log entries, and breadcrumb tracking with automatic file/function/line context.

## Installation

1. Copy the `HookDebugLog` folder into your project's `Plugins/` directory
2. Restart the Unreal Editor so it detects the new plugin

## Quick Start (Per-File Include)

Add `"HookDebugLog"` to your module's `PublicDependencyModuleNames` in your `.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "HookDebugLog" });
```

Then include the header in any `.cpp` file where you want to use it:

```cpp
#include "HookDebugLog.h"

HOOK_LOG(EHookLogType::Info, TEXT("Hello from HookDebugLog!"));
```

## Recommended Setup (Module-Wide, No Includes Needed)

With a one-time setup, `HOOK_LOG` becomes available in every `.cpp` file in your module automatically, including any new files you create later. No per-file `#include` needed.

### Step 1: Add the Module Dependency

Open your game module's `.Build.cs` (e.g., `Source/MyGame/MyGame.Build.cs`) and add `"HookDebugLog"` to your public dependencies:

```csharp
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "HookDebugLog" });
```

### Step 2: Set Up the Private PCH

In the same `.Build.cs` file, add the `PrivatePCHHeaderFile` line right after your `PCHUsage` line:

```csharp
PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
PrivatePCHHeaderFile = "MyGame.h";
```

Replace `"MyGame.h"` with your module's main header file name (the `.h` file that sits next to your `.Build.cs`).

### Step 3: Include HookDebugLog in Your Module Header

Open your module header (e.g., `Source/MyGame/MyGame.h`) and add the include:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "HookDebugLog.h"
```

### Step 4: Build and Use

Build your project. In any `.cpp` file, use `HOOK_LOG` directly:

```cpp
HOOK_LOG(EHookLogType::Info, TEXT("Player spawned"));
HOOK_LOG(EHookLogType::Warning, TEXT("Health is low"));
HOOK_LOG(EHookLogType::Error, TEXT("Failed to load save data"));
HOOK_LOG(EHookLogType::Success, TEXT("Connected to server"));
HOOK_LOG_QUIET(); // breadcrumb-only log, no screen output
```

### Why This Works

`PrivatePCHHeaderFile` tells Unreal Build Tool to precompile your module header and force-include it in every `.cpp` file in your module. Since your module header includes `HookDebugLog.h`, the `HOOK_LOG` and `HOOK_LOG_QUIET` macros are automatically available everywhere.

## Log Types

| Type | Color | Description |
|------|-------|-------------|
| `EHookLogType::Success` | Green | Successful operations |
| `EHookLogType::Warning` | Yellow | Potential issues |
| `EHookLogType::Error` | Red | Errors and failures |
| `EHookLogType::Info` | White | General information |

## Custom Log Category (Optional)

By default, the log category is `{YourProjectName}Log`. To customize it, add this to your `Config/DefaultEngine.ini`:

```ini
[/Script/HookDebugLog.HookDebugLogConfig]
LogCategoryName=MyCustomLog
```

## Runtime Controls

HookDebugLog exposes console variables for runtime control:

| CVar | Default | Description |
|------|---------|-------------|
| `hook.DebugScreenLog` | 1 (editor) / 0 (non-editor) | Enable on-screen messages |
| `hook.ScreenShowFile` | 1 | Show filename in on-screen output |
| `hook.ScreenShowFunction` | 1 | Show function name in on-screen output |
| `hook.ScreenShowLine` | 1 | Show line number in on-screen output |

On-screen logging is automatically stripped in shipping builds.

## Editor Settings

Project Settings and Editor Preferences include a Debug Settings section where you can configure the same options through the UI. Changes sync to the console variables automatically.
