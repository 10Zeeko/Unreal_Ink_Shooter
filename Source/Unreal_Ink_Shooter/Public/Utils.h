#pragma once

#define ScreenD(x) if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, x); }
#define ScreenDv(x) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, 2.0f, FColor::Yellow, x);}

#define ScreenDt(x, y) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, y.0f, FColor::Yellow, TEXT(x));}
#define ScreenDv(x, y) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, y.0f, FColor::Yellow, x);}

#define LogD(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define LogD2(x, y) UE_LOG(LogTemp, Warning, TEXT(x), y);
#define LogDt(x) UE_LOG(LogTemp, Warning, x);

#define Format1(x, y) FString::Printf(TEXT(x), y)
#define Format2(x, y, z) FString::Printf(TEXT(x), y, z)
#define Format3(x, y, z, a) FString::Printf(TEXT(x), y, z, a)
#define Format4(x, y, z, a, b) FString::Printf(TEXT(x), y, z, a, b)
#define Format5(x, y, z, a, b, c) FString::Printf(TEXT(x), y, z, a, b, c)
