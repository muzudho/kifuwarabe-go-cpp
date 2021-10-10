#include "selfPlay.h"

void Selfplay(Position position, UpperConfidenceTree uct)
{
    // 黒の手番
    int color = 1;

    // 探索方法フラグ
    int search;

    for (;;)
    {
        // 黒番、白番ともにUCT探索
        if (color == 1)
        {
            search = kSearchUct; //kSearchPrimitive;
        }
        else
        {
            search = kSearchUct;
        }

        // 次の一手
        timeMan.SetUpStartTime(position.CountTotalTime());
        position.ClearBeforeComputerMove();

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
        position.SetUpExpendTime(color, z);

        // パスパスなら終局
        if (z == 0 && position.moves > 1 && position.record[position.moves - 2] == 0)
            break;

        // 300手を超えても終局
        if (position.moves > 300)
            break; // too long

        // 手番の色反転
        color = FlipColor(color);
    }

    // SGF形式で棋譜表示
    position.PrintSgf();
}
