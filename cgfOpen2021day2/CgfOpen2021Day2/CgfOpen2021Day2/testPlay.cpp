#include "testPlay.h"

void TestPlayout(Position position)
{
    // テストプレイであるというフラグを立てます
    position.flag_test_playout = 1;

    // 黒手番でプレイアウト
    position.Playout(1);

    // 盤表示
    position.PrintBoard();

    // SGF形式の棋譜を出力
    position.PrintSgf();
}
