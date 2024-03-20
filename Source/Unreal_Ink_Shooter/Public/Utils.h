#pragma once

#define ScreenD(x) if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, x); }
#define Format1(x, y) FString::Printf(TEXT(x), y)