#include <string.h>
#include "computerMove.h"
#include "randXorShift.h"
#include "upperConfidenceTree.h"

/// <summary>
/// コンピューターの指し手
/// </summary>
/// <param name="color">手番の色</param>
/// <param name="search">探索方法。SEARCH_PRIMITIVE または kSearchUct</param>
/// <returns>座標</returns>
int PlayComputerMove(Position position, int color, int search)
{
    // 秒
    double sec;

    // 着手点
    int z;

    // 累計時間
    double total_time = position.CountTotalTime();

    double base_time = 60 * 10; // 10 minutes
    double left_time = base_time - total_time;
    int div = 12; // 40 ... 13x13, 70 ... 19x19
    timeMan.time_limit_sec = left_time / div;
    if (left_time < 60)
        timeMan.time_limit_sec = 1.0;
    if (left_time < 20)
        timeMan.time_limit_sec = 0.2;
    Prt("time_limit_sec=%.1f, total=%.1f, left=%.1f\n", timeMan.time_limit_sec, total_time, left_time);

    // 開始時刻
    timeMan.start_time = timeMan.GetClock();

    // プレイアウト回数
    position.all_playouts = 0;

    // 盤領域をゼロ クリアー？
    memset(position.board_area_sum, 0, sizeof(position.board_area_sum));

    // 盤上の勝ち数をゼロ クリアー？
    memset(position.board_winner, 0, sizeof(position.board_winner));

    // 勝ち数をクリアー？
    memset(position.winner_count, 0, sizeof(position.winner_count));

    // Takahashi: UCT と 原始モンテカルロを乱数で切り替えます
    if (0 == Rand64() % 2) // search == kSearchUct
    {
        // UCTを使ったゲームプレイ
        z = position.GetBestUct(color);
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

    // 消費時間（秒）？
    sec = timeMan.GetSpendTime(timeMan.start_time);

    // 情報表示
    Prt("z=%s,color=%d,moves=%d,playouts=%d, %.1f sec(%.0f po/sec),depth=%d\n",
        GetCharZ(z), color, position.moves, position.all_playouts, sec, position.all_playouts / sec, depth);

    // 指し手を棋譜に記憶します
    position.AddMoves(z, color, sec);

    return z;
}
