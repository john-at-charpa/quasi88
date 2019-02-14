/************************************************************************/
/*                                  */
/*              QUASI88                 */
/*                                  */
/************************************************************************/

/*
 * ImmDisableIME() は、 WINVER >= 0x040A 限定のAPIらしい。(Win98/2K以降)
 * VC6 では、なにもしないと WINVER == 0x0400 となってしまうので、
 * windows.h をインクルードする前に、 WINVER を強制的に指定してしまおう
 */
#ifdef  _MSC_VER
#ifndef WINVER
#define WINVER 0x040A
#endif
#if WINVER < 0x040A
#undef WINVER
#define WINVER 0x040A
#endif
#endif

extern "C"
{
    #include <stdio.h>
    #include <stdlib.h>

    #include "device.h"
    #include "quasi88.h"

    #include "getconf.h"    /* config_init */
    #include "keyboard.h"   /* romaji_type */
    #include "suspend.h"    /* stateload_system */
    #include "menu.h"   /* menu_about_osd_msg */
}

FILE    *debugfp;
/***********************************************************************
 * メイン処理
 ************************************************************************/
static  void    finish(void);

int WINAPI WinMain(HINSTANCE hInst,
           HINSTANCE hPrevInst,
           LPSTR pCmdLine,
           int showCmd)
{
    g_hInstance = hInst;


#ifndef NDEBUG
    /* コンソールを作成し，標準出力を割り当てる */
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif
/*
    debugfp = fopen("debug.txt", "w");
*/
    if (debugfp == NULL) { debugfp = stdout; }



#if 1
    /* IME を無効にしておく (imm.h, imm32.lib)      */
    /* ウインドウ生成前に呼び出す必要があるらしい  */
#if (WINVER >= 0x040A)
    ImmDisableIME(0);
#endif
#endif

    /* 一部の初期値を改変 (いいやり方はないかな…) */
    romaji_type = 1;            /* ローマ字変換の規則を MS-IME風に */

#ifndef __argc
#define __argc 0
#define __argv NULL
#endif

    if (config_init(__argc, __argv,        /* 環境初期化 & 引数処理 */
            NULL,
            NULL)) {

    quasi88_atexit(finish);     /* quasi88() 実行中に強制終了した際の
                       コールバック関数を登録する */

    quasi88();          /* PC-8801 エミュレーション */

    config_exit();          /* 引数処理後始末 */
    }

    return 0;
}



/*
 * 強制終了時のコールバック関数 (quasi88_exit()呼出時に、処理される)
 */
static  void    finish(void)
{
    config_exit();          /* 引数処理後始末 */
}



/***********************************************************************
 * ステートロード／ステートセーブ
 ************************************************************************/

/*  他の情報すべてがロード or セーブされた後に呼び出される。
 *  必要に応じて、システム固有の情報を付加してもいいかと。
 */

int stateload_system(void)
{
    return TRUE;
}
int statesave_system(void)
{
    return TRUE;
}



/***********************************************************************
 * メニュー画面に表示する、システム固有メッセージ
 ************************************************************************/

int menu_about_osd_msg(int        req_japanese,
               int        *result_code,
               const char *message[])
{
    static const char *about_en =
    {
    "Fullscreen mode not supported.\n"
    "Joystick not supported.\n"
    };

    static const char *about_jp =
    {
    "フルスクリーン表示はサポートされていません\n"
    "ジョイスティックはサポートされていません\n"
    "マウスカーソルの表示制御はサポートされていません\n"
    "キー設定ファイルの読み込みはサポートされていません\n"
    };


    *result_code = -1;              /* 文字コード指定なし */

    if (req_japanese == FALSE) {
    *message = about_en;
    } else {
    *message = about_jp;
    }

    return TRUE;
}
