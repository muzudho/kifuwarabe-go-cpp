#include "upperConfidenceTree.h"
#include <iostream>
#include "playerColorBoard.h"

extern Position position;

void UpperConfidenceTree::UpdateRave(Node* pN, int color, int current_depth, double win)
{
    // ループ カウンター
    int i;

    // 着手した座標
    int z;

    // 手番の色
    int c = color;

    // 盤面の各交点の手番の色？
    PlayerColorBoard player_color_board = PlayerColorBoard();
    // int _played_color[kBoardMax];
    // ゼロ クリアーした？
    // memset(player_color_board._played_color, 0, sizeof(player_color_board._played_color));

    // 経路の残りの負荷さについて
    for (i = current_depth; i < depth; i++)
    {
        // 着手点
        z = path[i];

        // 記録がなければ上書き
        if (player_color_board.IsEmpty(z)) {
            player_color_board.SetColor(z, c);
        }

        // 手番の色交代
        c = FlipColor(c);
    }

    // パスはゼロ クリアー
    // ignore pass
    player_color_board.SetColor(0, 0);

    // すべての子ノードについて
    for (i = 0; i < pN->child_num; i++)
    {
        // 子ノード
        Child* c = &pN->children[i];

        // 非合法手は無視
        if (c->z == kIllegalZ) {
            continue;
        }

        // 相手の色なら無視
        if (player_color_board.GetColor(c->z) != color) {
            continue;
        }

        // レーブ率の再計算？
        c->rave_rate = (c->rave_games * c->rave_rate + win) / (c->rave_games + 1);

        // レーブの対局数？をカウント
        c->rave_games++;

        // 子レーブの対局数？をカウント
        pN->child_rave_games_sum++;
    }
}

int UpperConfidenceTree::SearchUct(Position position, int color, int node_n)
{
    // この局面
    Node* pN = &nodeList[node_n];

    // 最善の一手（子ノード）
    Child* c = NULL;
    int select, z, err, win, current_depth;

    // とりあえず打ってみる
    for (;;)
    {
        // 最善の一手（子ノード）のインデックス
        select = SelectBestUcb(node_n, color);

        // 最善の一手（子ノード）
        c = &pN->children[select];

        // 最善の一手（子ノード）の座標
        z = c->z;

        // 石を置く
        err = position.PutStone(z, color, kFillEyeErr);

        // 合法手ならループを抜けます
        if (err == 0) {
            break;
        }

        // 非合法手なら、 kIllegalZ をセットして ループをやり直し
        // select other move
        c->z = kIllegalZ;
    }

    // 現在の深さを更新
    current_depth = depth;

    // 経路に着手点を格納
    path[depth++] = c->z;

    // この一手が１度も試行されていなければ、プレイアウトします
    // c->games <= 10 とかにすればメモリを節約できます。
    // c->games <= 0 より強くなる場合もあります。
    // 経路の深さが配列のサイズを超えているときもプレイアウトします。
    // 自分がパスを連続しているときもプレイアウトします。
    // playout in first time. <= 10 can reduce node.
    if (c->games <= 0 || depth == kDMax || (c->z == 0 && depth >= 2 && path[depth - 2] == 0))
    {
        // 手番をひっくり返してプレイアウト
        win = -position.Playout(FlipColor(color));
    }
    else
    {
        // 子ノードが葉なら、さらに延長
        if (c->next == NODE_EMPTY)
            c->next = CreateNode(c->z);

        // 手番をひっくり返して UCT探索（ネガマックス形式）。勝率はひっくり返して格納
        win = -SearchUct(position, FlipColor(color), c->next);
    }

    // レーブの更新
    UpdateRave(pN, color, current_depth, win);

    // 勝率の更新（update winrate）
    c->rate = (c->rate * c->games + win) / (c->games + 1);

    // 対局数カウントアップ
    c->games++;

    // 子の対局数？をカウントアップ
    pN->child_games_sum++;

    return win;
}

/// <summary>
/// UCBが最大の手を返します。
/// 一度も試していない手は優先的に選びます。
/// 定数 Ｃ は実験で決めてください。
/// PASS があるので、すべての手がエラーはありえません。
/// （UCTループ１回分の中で、定数倍ループ。m×n）
/// </summary>
/// <param name="node_n">ノードのリストのインデックス</param>
/// <param name="color">手番の色</param>
/// <returns>ノードのリストのインデックス。選択した子ノードを指します</returns>
int UpperConfidenceTree::SelectBestUcb(int node_n, int color)
{
    // この局面
    Node* pN = &nodeList[node_n];

    // 1手でも選ばれたか否か
    int select = -1;
    double max_ucb = -999;
    double ucb = 0, ucb_rave = 0, beta;

    // ループ カウンター
    int i;

    for (i = 0; i < pN->child_num; i++)
    {
        // この子ノード
        Child* c = &pN->children[i];

        // 非合法手の座標なら無視
        if (c->z == kIllegalZ)
        {
            continue;
        }

        // 試した回数が 0 のとき
        if (c->games == 0)
        {
            ucb_rave = 10000 + (rand() & 0x7fff); // try once
        }
        else
        {
            const double C = 0.30; // depends on program

            // 何らかのレーブの計算？
            const double RAVE_D = 3000;
            double moveCount = c->games;
            double raveCount = c->rave_games;
            double rave = c->rave_rate;
            if (c->z == 0)
            { // dont select pass
                rave = 1 - color;
                raveCount = pN->child_games_sum;
            }

            beta = raveCount / (raveCount + moveCount + raveCount * moveCount / RAVE_D);

            // UCBの計算
            ucb = c->rate + C * sqrt(log((double)pN->child_games_sum) / c->games);

            ucb_rave = beta * rave + (1 - beta) * ucb;
            // if ( depth==0 ) {
            // std::cerr << std::setw(2) << i
            //  << ":z=" << std::setw(2) << Get81(c->z)
            //  << ",rate=" << std::setw(6) << std::setprecision(3) << c->rate
            //  << ",games=" << std::setw(4) << c->games
            //  << ", rave_r=" << std::setw(6) << std::setprecision(3) << c->rave_rate
            //  << ",g=" << c->rave_games
            //  << ", beta=" << beta
            //  << ",ucb_rave=" << ucb_rave << std::endl;
            // }
        }

        // UCB値の最大を更新
        if (ucb_rave > max_ucb)
        {
            max_ucb = ucb_rave;
            select = i;
        }
    }

    // 1手も選ばれなかったらエラーなので強制終了
    if (select == -1)
    {
        std::cerr << "Err! select" << std::endl;
        exit(0);
    }

    return select;
}

int UpperConfidenceTree::GetBestUct(Position position, int color)
{
    int next, i, best_z, best_i = -1;

    // 手が打たれた対局数の最大数？
    int max = -999;

    // この局面
    Node* pN;

    // 前回の着手座標？
    int prev_z = 0;

    // 初手以外は前回の着手点を記憶
    if (position.moves > 0) {
        prev_z = position.record[position.moves - 1];
    }

    // ノードリストの要素数
    node_num = 0;

    // 次のノードのインデックス。ここでは0。現図を作成しています
    next = CreateNode(prev_z);

    // とりあえず UCT探索（search_uct）を、uct.loop回繰り返します
    for (i = 0; i < loop; i++)
    {
        // 現図を退避
        int board_copy[kBoardMax];
        int ko_z_copy;
        position.BackupCurrent(board_copy, &ko_z_copy);

        // 経路の深さを初期化
        depth = 0;

        // UCT探索
        SearchUct(position, color, next);

        // 現図を復元
        position.RestoreCurrent(board_copy, &ko_z_copy);

        // 時間切れなら抜けます
        if (timeMan.IsTimeOver())
            break;
    }

    // 次のノード
    pN = &nodeList[next];

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
        std::cerr << std::setw(2) << i
            << ":z=" << std::setw(2) << Get81(c->z)
            << ",rate=" << std::setw(6) << std::setprecision(3) << c->rate
            << ",games=" << std::setw(4) << c->games
            << ",rave_r=" << std::setw(6) << std::setprecision(3) << c->rave_rate
            << ",g=" << std::setw(4) << c->rave_games << std::endl;
#endif
    }

    // ベストなノードの座標
    best_z = pN->children[best_i].z;
#if DEBUG
    std::cerr << "best_z=" << Get81(best_z)
        << ",rate=" << std::setw(6) << std::setprecision(3) << pN->children[best_i].rate
        << ",games=" << std::setw(4) << max
        << ",playouts=" << all_playouts
        << ",nodes=" << uct.node_num << std::endl;
#endif

    return best_z;
}


/// <summary>
/// create new node.
/// 空点を全部追加。
/// PASSも追加。
/// </summary>
/// <param name="prev_z">前回の着手の座標</param>
/// <returns>ノードのリストのインデックス。作られたノードを指す。最初は0から</returns>
int UpperConfidenceTree::CreateNode(int prev_z)
{
    // 筋
    int x;

    // 段
    int y;

    // 着手点
    int z;

    // ループ カウンター
    int i, j;

    // この局面
    Node* pN;

    // これ以上増やせません
    if (node_num == kNodeMax)
    {
        std::cerr << "nodeList over Err" << std::endl;
        exit(0);
    }

    // 末尾の未使用の要素
    pN = &nodeList[node_num];
    pN->child_num = 0;
    pN->child_games_sum = 0;
    pN->child_rave_games_sum = 0;

    // 空点をリストの末尾に追加
    for (y = 0; y < kBoardSize; y++) {
        for (x = 0; x < kBoardSize; x++)
        {
            z = GetZ(x + 1, y + 1);
            if (position.Board[z] != 0)
                continue;
            pN->AddChild(z, 0);
        }
    }
    pN->AddChild(0, 0); // add PASS

    // sort children
    for (i = 0; i < pN->child_num - 1; i++)
    {
        double max_b = pN->children[i].bonus;
        int max_i = i;
        Child tmp;
        for (j = i + 1; j < pN->child_num; j++)
        {
            Child* c = &pN->children[j];
            if (max_b >= c->bonus)
                continue;
            max_b = c->bonus;
            max_i = j;
        }
        if (max_i == i)
            continue;
        tmp = pN->children[i];
        pN->children[i] = pN->children[max_i];
        pN->children[max_i] = tmp;
    }

    // 末尾に１つ追加した分、リストのサイズ１つ追加
    node_num++;

    // 最後の要素を指すインデックスを返します
    return node_num - 1;
}
