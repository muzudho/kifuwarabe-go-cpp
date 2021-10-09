#include <string.h>
#include "computerMove.h"
#include "randXorShift.h"
#include "upperConfidenceTree.h"

int PlayComputerMove(Position position, UpperConfidenceTree uct, int color, int search)
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
        z = GetBestUct(position, uct, color);
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

int GetBestUct(Position position, UpperConfidenceTree uct, int color)
{
    int next, i, best_z, best_i = -1;

    // 手が打たれた対局数の最大数？
    int max = -999;

    // この局面
    Node* pN;

    // 前回の着手座標？
    int prev_z = 0;

    // 初手以外は前回の着手点を記憶
    if (position.moves > 0)
        prev_z = position.record[position.moves - 1];

    // ノードリストの要素数
    uct.node_num = 0;

    // 次のノードのインデックス。ここでは0。現図を作成しています
    next = uct.CreateNode(prev_z);

    // とりあえず UCT探索（search_uct）を、uct.loop回繰り返します
    for (i = 0; i < uct.loop; i++)
    {
        // 現図を退避
        int board_copy[kBoardMax];
        int ko_z_copy = position.ko_z;
        memcpy(board_copy, position.Board, sizeof(position.Board));

        // 経路の深さを初期化
        depth = 0;

        // UCT探索
        uct.SearchUct(position, color, next);

        // 現図を復元
        position.ko_z = ko_z_copy;
        memcpy(position.Board, board_copy, sizeof(position.Board));

        // 時間切れなら抜けます
        if (timeMan.IsTimeOver())
            break;
    }

    // 次のノード
    pN = &uct.nodeList[next];

    // 子ノード全部確認
    for (i = 0; i < pN->child_num; i++)
    {
        // 子ノード
        Child* c = &pN->children[i];

        // 最大対局数（一番打たれた手ということ）の更新
        if (c->games > max)
        {
            best_i = i;
            max = c->games;
        }
#if DEBUG
        Prt("%2d:z=%2d,rate=%6.3f,games=%4d, rave_r=%6.3f,g=%4d\n",
            i, Get81(c->z), c->rate, c->games, c->rave_rate, c->rave_games);
#endif
    }

    // ベストなノードの座標
    best_z = pN->children[best_i].z;
#if DEBUG
    Prt("best_z=%d,rate=%6.3f,games=%4d,playouts=%d,nodes=%d\n",
        Get81(best_z), pN->children[best_i].rate, max, all_playouts, uct.node_num);
#endif

    return best_z;
}