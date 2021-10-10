// CgfOpen2021Day2.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <sys/stat.h> // setvbuf

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します

// sprintf is Err in VC++
#define _CRT_SECURE_NO_WARNINGS

#include "go15.h"
#include "child.h"
#include "node.h"
#include "upperConfidenceTree.h"
#include "position.h"
#include "common.h"
#include "randXorShift.h"
#include "selfPlay.h"
#include "testPlay.h"

Position position = Position();

/// <summary>
/// UCTで使われる経路（パス）
/// プレイアウトでも使う
/// </summary>
int path[kDMax];

/// <summary>
/// UCTで使われる経路（パス）の先頭がらn番目(0開始)
/// プレイアウトでも使う
/// </summary>
int depth;

/// <summary>
/// プレイアウトします
/// </summary>
/// <param name="turn_color">手番の石の色</param>
/// <returns>黒の勝ちなら1、負けなら0</returns>
int Position::Playout(int turn_color)
{
    // 手番の色
    int color = turn_color;

    // １つ前の着手の座標
    int previous_z = 0;

    // ループ・カウンタ
    int loop;
    int loop_max = kBoardSize * kBoardSize + 200; // for triple ko

    // プレイアウト回数のカウント
    all_playouts++;

    for (loop = 0; loop < loop_max; loop++)
    {
        // all empty points are candidates.
        // [0]...z, [1]...probability
        int empty[kBoardMax][2];

        // 配列のインデックス
        int empty_num = 0;

        // 空点を選ぶ確率？
        int prob_sum = 0;
        // 壁を除く盤上の全ての空点の(座標,確率)を empty配列にセットします
        int x, y, z, err, pr;
        for (y = 0; y < kBoardSize; y++) {
            for (x = 0; x < kBoardSize; x++)
            {
                int z = GetZ(x + 1, y + 1);
                if (Board[z] != 0) {
                    continue;
                }

                empty[empty_num][0] = z;
                // 空点を選ぶ確率？
                pr = 100;
                //    pr = get_prob(z, previous_z, color);
                empty[empty_num][1] = pr;
                prob_sum += pr;
                empty_num++;
            }
        }

        for (;;)
        {
            int i = 0;
            // もし空点がなければ、パス
            if (empty_num == 0)
            {
                z = 0;
            }
            else
            {
                // 確率的に空点を選んでいる？
                int r = rand() % prob_sum;
                int sum = 0;
                for (i = 0; i < empty_num; i++)
                {
                    sum += empty[i][1]; // 0,1,2   [0]=1, [1]=1, [2]=1
                    if (sum > r) {
                        break;
                    }
                }

                if (i == empty_num)
                {
                    std::cerr << "Err! prob_sum=" << prob_sum
                        << ",sum=" << sum
                        << ",r=" << r
                        << ",i=" << i << std::endl;
                    exit(0);
                }
                z = empty[i][0];
            }

            err = PutStone(z, color, kFillEyeErr);

            if (err == 0) {
                break; // pass is ok.
            }

            // もし空点に石を置くと正常終了しなかったなら、残りの座標で続行します
            prob_sum -= empty[i][1];
            empty[i][0] = empty[empty_num - 1][0]; // err, delete
            empty[i][1] = empty[empty_num - 1][1];
            empty_num--;
        }

        // テストでプレイアウトするのなら、棋譜に手を記録します
        if (flag_test_playout) {
            record[moves++] = z;
        }

        // 経路（パス）の深さに配列サイズ上まだ余裕があれば、着手点を記憶します
        if (depth < kDMax) {
            path[depth++] = z;
        }

        // もしパスが連続したら対局終了
        if (z == 0 && previous_z == 0) {
            break; // continuous pass
        }

        // そうでなければ盤を表示して手番を変えて続行
        previous_z = z;
        // std::cerr << "loop=" << loop
        //  << ",z=" << GetCharZ(z)
        //  << ",c=" << color
        //  << ",empty_num=" << empty_num
        //  << ",ko_z=" << ko_z << std::endl;

        // 手番を反転
        color = FlipColor(color);
    }

    return CountScore(turn_color);
}

/// <summary>
/// 原始モンテカルロ（ネガマックス）
/// </summary>
/// <param name="color">手番の色</param>
/// <returns>最善手の座標</returns>
int Position::PrimitiveMonteCalro(int color)
{
    // Takahashi: 消費時間を短縮したい。でも6秒だとペンキ塗りしてしまう。
    // Takahashi: 8でちょうどいいかと思ったが、石の上に石を置いたかもしれないので、もう少し長めにする。
    // Takahashi: 20だと 持ち時間 30分でちょうど良さそうだが、それでも石の上に石を置いた。
    // number of playout
    int try_num = 15; // 30;

    // 最善手の着手点
    int best_z = 0;

    // 最善の価値
    double best_value;

    // 勝率
    double win_rate;

    // 筋
    int x;

    // 段
    int y;

    // エラーコード
    int err;

    // ループ カウンター
    int i;

    // プレイアウトして勝った回数
    int win_sum;

    // 手番が勝ったら1、負けたら0
    int win;


    // 盤のコピー
    // keep current board
    int board_copy[kBoardMax];
    // コウの座標のコピー
    int ko_z_copy;
    BackupCurrent(board_copy, &ko_z_copy);

    // 根ノードでは最低-100点から
    best_value = -100;

    // try all empty point
    for (y = 0; y < kBoardSize; y++) {
        for (x = 0; x < kBoardSize; x++)
        {
            // 石を置く座標
            int z = GetZ(x + 1, y + 1);

            // 空点でなければ無視
            if (Board[z] != 0) {
                continue;
            }

            // 目潰ししないように石を置く
            err = PutStone(z, color, kFillEyeErr);
            if (err != 0) {
                continue;
            }

            // 勝った回数
            win_sum = 0;
            for (i = 0; i < try_num; i++)
            {
                // 現局面を退避
                int board_copy2[kBoardMax];
                int ko_z_copy2;
                BackupCurrent(board_copy2, &ko_z_copy2);

                // プレイアウト
                win = -Playout(FlipColor(color));
                win_sum += win;

                // 現局面に復元
                RestoreCurrent(board_copy2, &ko_z_copy2);
            }

            // 勝率
            win_rate = (double)win_sum / try_num;
            //  PrintBoard();
            // std::cerr << "z=" << Get81(z) 
            // << ",win=" <<  std::setw( 5 ) << std::setprecision( 3 ) << win_rate << std::endl;

            // 最善手の更新
            if (win_rate > best_value)
            {
                best_value = win_rate;
                best_z = z;
                // std::cerr << "best_z=" << Get81(best_z)
                //  << ",color=" << color
                //  << ",v=" << std::setw( 5 ) << std::setprecision( 3 ) << best_value
                //  << ",try_num=" try_num << std::endl;
            }
        }
    }

    // Takahashi: for文のコードブロックの外に移動
    RestoreCurrent(board_copy, &ko_z_copy);

    return best_z;
}

// following are for UCT(Upper Confidence Tree)
// `UCT` - 探索と知識利用のバランスを取る手法

// 以下、探索木全体を保存
UpperConfidenceTree uct = UpperConfidenceTree();

TimeMan timeMan = TimeMan();

// GTPプロトコルの1行を読込むのに十分な文字列サイズ
const int kStrMax = 256;

// スペース区切りで3つ分まで読込む？
const int kTokenMax = 3;

/// <summary>
/// GTPプロトコルのループ
/// </summary>
void GtpLoop()
{
    // 入力文字列バッファー
    char str[kStrMax];

    // スプリットされた文字列のリスト
    char sa[kTokenMax][kStrMax];

    // コマンドはスペース区切り
    char delim[] = " ";

    int x, y, z, ax, count;

    // 難しいのでコメントアウトします
    // 標準出力をバッファリングせず、すぐに出力する設定（フラッシュを不要にします）
    // setbuf(stdout, NULL);
    // setvbuf(stdout, NULL, _IONBF, NULL);

    // 難しいのでコメントアウトします
    // 標準エラー出力をバッファリングせず、すぐに出力する設定（フラッシュを不要にします）
    // setbuf(stderr, NULL);
    // setvbuf(stderr, NULL, _IONBF, NULL);

    for (;;)
    {
        // 標準入力から文字列読取
        if (fgets(str, kStrMax, stdin) == NULL)
            break;

        std::cerr << "gtp<-" << str;

        // 文字列のスプリットをして、結果を sa配列に格納しています
        count = 0;

        // スプリットされた文字列１つ
        char* token;
        // token = strtok(str, delim);
        // strtok_sの内部で使用します
        char* context;
        token = strtok_s(str, delim, &context);

        while (token != NULL)
        {
            // strcpy(sa[count], token);
            strcpy_s(sa[count], kStrMax, token);

            count++;
            if (count == kTokenMax)
                break;

            // 2回目以降は第一引数をNULLにします
            // token = strtok(NULL, delim);
            token = strtok_s(NULL, delim, &context);
        }

        // 盤のサイズを n路 にしてください
        if (strstr(sa[0], "boardsize"))
        {
            // 無視
            //    int new_board_size = atoi( sa[1] );
            std::cout << "= " << std::endl << std::endl;
        }
        // 盤を初期化してください
        else if (strstr(sa[0], "clear_board"))
        {
            position.InitBoard();
            std::cout << "= " << std::endl << std::endl;
        }
        // 思考エンジンを終了してください
        else if (strstr(sa[0], "quit"))
        {
            break;
        }
        // GTPプロトコルのバージョンを返してください
        else if (strstr(sa[0], "protocol_version"))
        {
            std::cout << "= 2" << std::endl << std::endl;
        }
        // 思考エンジンの名前を返してください
        else if (strstr(sa[0], "name"))
        {
            // your_program_name
            std::cout << "= kif" << std::endl << std::endl;
        }
        // 思考エンジンのバージョンを返してください
        else if (strstr(sa[0], "version"))
        {
            std::cout << "= 0.0.1" << std::endl << std::endl;
        }
        // 対応しているコマンドの改行区切りリストを返してください
        else if (strstr(sa[0], "list_commands"))
        {
            std::cout << "= boardsize" << std::endl
                << "clear_board" << std::endl
                << "quit" << std::endl
                << "protocol_version" << std::endl
                << "name" << std::endl
                << "version" << std::endl
                << "list_commands" << std::endl
                << "komi" << std::endl
                << "genmove" << std::endl
                << "play" << std::endl << std::endl;
        }
        // コミを ｘ にしてください
        else if (strstr(sa[0], "komi"))
        {
            position.komi = atof(sa[1]);
            std::cout << "= " << std::endl << std::endl;
        }
        // あなたが着手する座標を返してください
        else if (strstr(sa[0], "genmove"))
        {
            int color = 1;
            if (tolower(sa[1][0]) == 'w')
                color = 2;

            timeMan.SetUpStartTime(position.CountTotalTime());
            position.ClearBeforeComputerMove();

            // 着手点
            int z = 0;
            if (kBoardSize == 19) {
                const int topLeftStar = GetZ(4, 4);
                const int topStar = GetZ(10, 4);
                const int topRightStar = GetZ(16, 4);
                const int leftStar = GetZ(4, 10);
                const int centerStarZ = GetZ(10, 10);
                const int rightStar = GetZ(16, 10);
                const int bottomLeftStar = GetZ(4, 16);
                const int bottomStar = GetZ(10, 16);
                const int bottomRightStar = GetZ(16, 16);
                //
                const int middleTopLeftStar = GetZ(7, 7);
                const int middleTopRightStar = GetZ(13, 7);
                const int middleBottomLeftStar = GetZ(7, 13);
                const int middleBottomRightStar = GetZ(13, 13);
                //
                // 決め打ち
                const int pattern_step1 = GetZ(7, 4);
                const int pattern_step2 = GetZ(13, 4);
                const int pattern_step3 = GetZ(16, 7);
                const int pattern_step4 = GetZ(16, 14);
                const int pattern_step5 = GetZ(13, 16);
                const int pattern_step6 = GetZ(7, 16);
                const int pattern_step7 = GetZ(4, 13);
                const int pattern_step8 = GetZ(4, 7);
                // 
                // 天元が空いていたら天元に打ちます（目に打たないように適当に右隣も空であることを確認します）
                if (position.Board[centerStarZ] == 0 && position.Board[centerStarZ + 1] == 0) {
                    z = centerStarZ;
                }
                // 右上の星
                else if (position.Board[topRightStar] == 0 && position.Board[topRightStar + 1] == 0) {
                    z = topRightStar;
                }
                // 左下の星
                else if (position.Board[bottomLeftStar] == 0 && position.Board[bottomLeftStar + 1] == 0) {
                    z = bottomLeftStar;
                }
                // 左上の星
                else if (position.Board[topLeftStar] == 0 && position.Board[topLeftStar + 1] == 0) {
                    z = topLeftStar;
                }
                // 右下の星
                else if (position.Board[bottomRightStar] == 0 && position.Board[bottomRightStar + 1] == 0) {
                    z = bottomRightStar;
                }
                // 上の星
                else if (position.Board[topStar] == 0 && position.Board[topStar + 1] == 0) {
                    z = topStar;
                }
                // 下の星
                else if (position.Board[bottomStar] == 0 && position.Board[bottomStar + 1] == 0) {
                    z = bottomStar;
                }
                // 右の星
                else if (position.Board[rightStar] == 0 && position.Board[rightStar + 1] == 0) {
                    z = rightStar;
                }
                // 左の星
                else if (position.Board[leftStar] == 0 && position.Board[leftStar + 1] == 0) {
                    z = leftStar;
                }
                // 中段の右上の星
                else if (position.Board[middleTopRightStar] == 0 && position.Board[middleTopRightStar + 1] == 0) {
                    z = middleTopRightStar;
                }
                // 中段の左下の星
                else if (position.Board[middleBottomLeftStar] == 0 && position.Board[middleBottomLeftStar + 1] == 0) {
                    z = middleBottomLeftStar;
                }
                // 中段の左上の星
                else if (position.Board[middleTopLeftStar] == 0 && position.Board[middleTopLeftStar + 1] == 0) {
                    z = middleTopLeftStar;
                }
                // 中段の右下の星
                else if (position.Board[middleBottomRightStar] == 0 && position.Board[middleBottomRightStar + 1] == 0) {
                    z = middleBottomRightStar;
                }
                // パターン1
                else if (position.Board[pattern_step1] == 0 && position.Board[pattern_step1 + 1] == 0) {
                    z = pattern_step1;
                }
                // パターン2
                else if (position.Board[pattern_step2] == 0 && position.Board[pattern_step2 + 1] == 0) {
                    z = pattern_step2;
                }
                // パターン3
                else if (position.Board[pattern_step3] == 0 && position.Board[pattern_step3 + 1] == 0) {
                    z = pattern_step3;
                }
                // パターン4
                else if (position.Board[pattern_step4] == 0 && position.Board[pattern_step4 + 1] == 0) {
                    z = pattern_step4;
                }
                // パターン5
                else if (position.Board[pattern_step5] == 0 && position.Board[pattern_step5 + 1] == 0) {
                    z = pattern_step5;
                }
                // パターン6
                else if (position.Board[pattern_step6] == 0 && position.Board[pattern_step6 + 1] == 0) {
                    z = pattern_step6;
                }
                // パターン7
                else if (position.Board[pattern_step7] == 0 && position.Board[pattern_step7 + 1] == 0) {
                    z = pattern_step7;
                }
                // パターン8
                else if (position.Board[pattern_step8] == 0 && position.Board[pattern_step8 + 1] == 0) {
                    z = pattern_step8;
                }
            }

            if (z == 0) {
                // Takahashi: UCT と 原始モンテカルロを乱数で切り替えます
                // Takahashi: ２つ使うと石の上に石置くかも？
                // Takahashi: UCT使うと石の上に石を置いた
                // Takahashi: 原始モンテカルロでも 石の上に石を置いた
                if (0 == Rand64() % 4) // search == kSearchUct
                {
                    // UCTを使ったゲームプレイ
                    std::cerr << "Use uct." << std::endl;
                    z = uct.GetBestUct(position, color);
                }
                else
                {
                    // 原始モンテカルロでも２３秒ぐらいかかってしまう
                    // 原始モンテカルロでゲームプレイ
                    std::cerr << "Use primitive monte calro." << std::endl;
                    z = position.PrimitiveMonteCalro(color);
                }
            }

            // 盤領域を表示？
            //PrintBoardArea();
            // クリティカルさを表示？
            //PrintCriticality();
            position.SetUpExpendTime(color, z);
            std::cout << "= " << GetCharZ(z) << std::endl << std::endl;
        }
        // 石の色と座標を指定しますので、置いてください
        else if (strstr(sa[0], "play"))
        { // "play b c4", "play w d17"
            int color = 1;
            if (tolower(sa[1][0]) == 'w')
                color = 2;
            ax = tolower(sa[2][0]);
            x = ax - 'a' + 1;
            if (ax >= 'i')
                x--;
            y = atoi(&sa[2][1]);
            z = GetZ(x, kBoardSize - y + 1);
            if (tolower(sa[2][0]) == 'p')
                z = 0; // pass
            position.AddMoves(z, color, 0);
            std::cout << "= " << std::endl << std::endl;
        }
        // その他のコマンドには未対応です
        else
        {
            std::cout << "? unknown_command" << std::endl << std::endl;
        }
    }
}

/// <summary>
/// プログラムはここから始まります
/// </summary>
/// <returns>エラーコード。正常時は0</returns>
int main()
{
    // 乱数の種を設定
    //srand( (unsigned)time( NULL ) );

    // 盤の初期化
    position.InitBoard();

    // ハッシュボードの初期化
    position.hashCode.MakeHashboard();

    // 自己対戦
    if (0)
    {
        Selfplay(position, uct);
        return 0;
    }

    // プレイアウトのテスト
    if (0)
    {
        TestPlayout(position);
        return 0;
    }

    // GTPプロトコルのループ
    GtpLoop();

    return 0;
}
