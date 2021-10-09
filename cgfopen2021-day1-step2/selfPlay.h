#pragma once

#include "go15.h"
#include "position.h"

/// <summary>
/// 黒番は原始モンテカルロ、白番はUCTで自己対戦
/// </summary>
void Selfplay(Position position);
