#include "computerMove.h"

int PlayComputerMove(Position position, UpperConfidenceTree uct, int color, int search)
{
    // 着手点
    int z;

    // Takahashi: UCT と 原始モンテカルロを乱数で切り替えます
    if (0 == Rand64() % 2) // search == kSearchUct
    {
        // UCTを使ったゲームプレイ
        z = uct.GetBestUct(position, color);
    }
    else
    {
        // 原始モンテカルロでゲームプレイ
        z = position.PrimitiveMonteCalro(color);
    }

    // 盤領域を表示？
    //PrintBoardArea();

    // クリティカルさを表示？
    //PrintCriticality();

    return z;
}
