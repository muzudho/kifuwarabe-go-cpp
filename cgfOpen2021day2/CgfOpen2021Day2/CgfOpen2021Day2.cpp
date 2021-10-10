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
