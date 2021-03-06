
// This file is part of the LITIV framework; visit the original repository at
// https://github.com/plstcharles/litiv for more information.
//
// Copyright 2015 Pierre-Luc St-Charles; pierre-luc.st-charles<at>polymtl.ca
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "litiv/utils/cxx.hpp"

/**
 *        console.hpp (litiv version inspired from rlutil.h)
 *
 *            rlutil.h: Copyright (C) 2010 Tapio Vierros
 *  -- see https://github.com/tapio/rlutil for more information --
 *      (originally distributed under the WTF public license)
 */

// define this to use ANSI escape sequences also on Windows (defaults to using WinAPI otherwise)
#if USE_RLUTIL_ANSI_DEFINE
#define RLUTIL_USE_ANSI
#endif //USE_RLUTIL_ANSI_DEFINE
#ifdef _WIN32
#include <windows.h>  // for WinAPI and Sleep()
#define _NO_OLDNAMES  // for MinGW compatibility
#include <conio.h>    // for getch() and kbhit()
#define getch _getch
#define kbhit _kbhit
#else //ndef _WIN32
#include <cstdio> // for getch()
#include <termios.h> // for getch() and kbhit()
#include <unistd.h> // for getch(), kbhit() and (u)sleep()
#include <sys/ioctl.h> // for getkey()
#include <sys/types.h> // for kbhit()
#include <sys/time.h> // for kbhit()

// get character without waiting for return to be pressed; windows has this in conio.h
inline int getch(void) {
    // Here be magic.
    struct termios oldt,newt;
    int ch;
    tcgetattr(STDIN_FILENO,&oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    return ch;
}

// determines if keyboard has been hit; windows has this in conio.h
inline int kbhit(void) {
    // Here be dragons.
    static struct termios oldt,newt;
    int cnt = 0;
    tcgetattr(STDIN_FILENO,&oldt);
    newt = oldt;
    newt.c_lflag    &= ~(ICANON | ECHO);
    newt.c_iflag     = 0; // input mode
    newt.c_oflag     = 0; // output mode
    newt.c_cc[VMIN]  = 1; // minimum time to wait
    newt.c_cc[VTIME] = 1; // minimum characters to wait for
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    ioctl(0,FIONREAD,&cnt); // Read count
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 100;
    select(STDIN_FILENO+1,NULL,NULL,NULL,&tv); // A small time delay
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    return cnt; // Return number of characters
}

#endif //ndef _WIN32

namespace rlutil {

#ifndef RLUTIL_STRING_TYPE
    typedef std::string str_t;
#else //def(RLUTIL_STRING_TYPE)
    typedef RLUTIL_STRING_TYPE str_t;
#endif //ndef RLUTIL_STRING_TYPE

    enum ColorCode {
        Color_BLACK=0,
        Color_BLUE,
        Color_GREEN,
        Color_CYAN,
        Color_RED,
        Color_MAGENTA,
        Color_BROWN,
        Color_GREY,
        Color_DARKGREY,
        Color_LIGHTBLUE,
        Color_LIGHTGREEN,
        Color_LIGHTCYAN,
        Color_LIGHTRED,
        Color_LIGHTMAGENTA,
        Color_BOLDYELLOW,
        Color_BOLDWHITE,
        Color_RESET,
        // codes below are not supported on windows
        Color_BOLDGREEN,
        Color_BOLDRED,
    };

    /**
     * ANSI color strings
     *
     * ANSI_CLS - Clears screen
     * ANSI_BLACK - Black
     * ANSI_RED - Red
     * ANSI_GREEN - Green
     * ANSI_BROWN - Brown / dark yellow
     * ANSI_BLUE - Blue
     * ANSI_MAGENTA - Magenta / purple
     * ANSI_CYAN - Cyan
     * ANSI_GREY - Grey / dark white
     * ANSI_DARKGREY - Dark grey / light black
     * ANSI_LIGHTRED - Light red
     * ANSI_LIGHTGREEN - Light green
     * ANSI_BOLDYELLOW - Yellow (bold/bright)
     * ANSI_LIGHTBLUE - Light blue
     * ANSI_LIGHTMAGENTA - Light magenta / light purple
     * ANSI_LIGHTCYAN - Light cyan
     * ANSI_BOLDWHITE - White (bold/bright)
     * ANSI_RESET - Resets formatting
     */
    constexpr str_t ANSI_CLS = "\033[2J";
    constexpr str_t ANSI_BLACK = "\033[22;30m";
    constexpr str_t ANSI_RED = "\033[22;31m";
    constexpr str_t ANSI_BOLDRED = "\033[22;31;1m";
    constexpr str_t ANSI_GREEN = "\033[22;32m";
    constexpr str_t ANSI_BOLDGREEN = "\033[22;32;1m";
    constexpr str_t ANSI_BROWN = "\033[22;33m";
    constexpr str_t ANSI_BLUE = "\033[22;34m";
    constexpr str_t ANSI_MAGENTA = "\033[22;35m";
    constexpr str_t ANSI_CYAN = "\033[22;36m";
    constexpr str_t ANSI_GREY = "\033[22;37m";
    constexpr str_t ANSI_DARKGREY = "\033[01;30m";
    constexpr str_t ANSI_LIGHTRED = "\033[01;31m";
    constexpr str_t ANSI_LIGHTGREEN = "\033[01;32m";
    constexpr str_t ANSI_BOLDYELLOW = "\033[01;33m";
    constexpr str_t ANSI_LIGHTBLUE = "\033[01;34m";
    constexpr str_t ANSI_LIGHTMAGENTA = "\033[01;35m";
    constexpr str_t ANSI_LIGHTCYAN = "\033[01;36m";
    constexpr str_t ANSI_BOLDWHITE = "\033[01;37m";
    constexpr str_t ANSI_RESET = "\033[39;49m\033[0m";

    /**
     * Key codes for keyhit()
     *
     * KEY_ESCAPE  - Escape
     * KEY_ENTER   - Enter
     * KEY_SPACE   - Space
     * KEY_INSERT  - Insert
     * KEY_HOME    - Home
     * KEY_END     - End
     * KEY_DELETE  - Delete
     * KEY_PGUP    - PageUp
     * KEY_PGDOWN  - PageDown
     * KEY_UP      - Up arrow
     * KEY_DOWN    - Down arrow
     * KEY_LEFT    - Left arrow
     * KEY_RIGHT   - Right arrow
     * KEY_F1      - F1
     * KEY_F2      - F2
     * KEY_F3      - F3
     * KEY_F4      - F4
     * KEY_F5      - F5
     * KEY_F6      - F6
     * KEY_F7      - F7
     * KEY_F8      - F8
     * KEY_F9      - F9
     * KEY_F10     - F10
     * KEY_F11     - F11
     * KEY_F12     - F12
     * KEY_NUMDEL  - Numpad del
     * KEY_NUMPAD0 - Numpad 0
     * KEY_NUMPAD1 - Numpad 1
     * KEY_NUMPAD2 - Numpad 2
     * KEY_NUMPAD3 - Numpad 3
     * KEY_NUMPAD4 - Numpad 4
     * KEY_NUMPAD5 - Numpad 5
     * KEY_NUMPAD6 - Numpad 6
     * KEY_NUMPAD7 - Numpad 7
     * KEY_NUMPAD8 - Numpad 8
     * KEY_NUMPAD9 - Numpad 9
     */
    constexpr int KEY_ESCAPE  = 0;
    constexpr int KEY_ENTER   = 1;
    constexpr int KEY_SPACE   = 32;

    constexpr int KEY_INSERT  = 2;
    constexpr int KEY_HOME    = 3;
    constexpr int KEY_PGUP    = 4;
    constexpr int KEY_DELETE  = 5;
    constexpr int KEY_END     = 6;
    constexpr int KEY_PGDOWN  = 7;

    constexpr int KEY_UP      = 14;
    constexpr int KEY_DOWN    = 15;
    constexpr int KEY_LEFT    = 16;
    constexpr int KEY_RIGHT   = 17;

    constexpr int KEY_F1      = 18;
    constexpr int KEY_F2      = 19;
    constexpr int KEY_F3      = 20;
    constexpr int KEY_F4      = 21;
    constexpr int KEY_F5      = 22;
    constexpr int KEY_F6      = 23;
    constexpr int KEY_F7      = 24;
    constexpr int KEY_F8      = 25;
    constexpr int KEY_F9      = 26;
    constexpr int KEY_F10     = 27;
    constexpr int KEY_F11     = 28;
    constexpr int KEY_F12     = 29;

    constexpr int KEY_NUMDEL  = 30;
    constexpr int KEY_NUMPAD0 = 31;
    constexpr int KEY_NUMPAD1 = 127;
    constexpr int KEY_NUMPAD2 = 128;
    constexpr int KEY_NUMPAD3 = 129;
    constexpr int KEY_NUMPAD4 = 130;
    constexpr int KEY_NUMPAD5 = 131;
    constexpr int KEY_NUMPAD6 = 132;
    constexpr int KEY_NUMPAD7 = 133;
    constexpr int KEY_NUMPAD8 = 134;
    constexpr int KEY_NUMPAD9 = 135;

    // reads a key press (blocking) and returns a key code; see key codes for info (note: only Arrows, Esc, Enter and Space are currently working properly)
    inline int getkey(void) {
#ifndef _WIN32
        int cnt = kbhit(); // for ANSI escapes processing
#endif //ndef _WIN32
        int k = getch();
        switch(k) {
            case 0: {
                int kk;
                switch (kk = getch()) {
                    case 71: return KEY_NUMPAD7;
                    case 72: return KEY_NUMPAD8;
                    case 73: return KEY_NUMPAD9;
                    case 75: return KEY_NUMPAD4;
                    case 77: return KEY_NUMPAD6;
                    case 79: return KEY_NUMPAD1;
                    case 80: return KEY_NUMPAD4;
                    case 81: return KEY_NUMPAD3;
                    case 82: return KEY_NUMPAD0;
                    case 83: return KEY_NUMDEL;
                    default: return kk-59+KEY_F1; // Function keys
                }}
            case 224: {
                int kk;
                switch (kk = getch()) {
                    case 71: return KEY_HOME;
                    case 72: return KEY_UP;
                    case 73: return KEY_PGUP;
                    case 75: return KEY_LEFT;
                    case 77: return KEY_RIGHT;
                    case 79: return KEY_END;
                    case 80: return KEY_DOWN;
                    case 81: return KEY_PGDOWN;
                    case 82: return KEY_INSERT;
                    case 83: return KEY_DELETE;
                    default: return kk-123+KEY_F1; // Function keys
                }}
            case 13: return KEY_ENTER;
#ifdef _WIN32
            case 27: return KEY_ESCAPE;
#else //ndef _WIN32
            case 155: // single-character CSI
            case 27: {
                // Process ANSI escape sequences
                if(cnt >= 3 && getch() == '[') {
                    switch (k = getch()) {
                        case 'A': return KEY_UP;
                        case 'B': return KEY_DOWN;
                        case 'C': return KEY_RIGHT;
                        case 'D': return KEY_LEFT;
                    }
                } else return KEY_ESCAPE;
            }
#endif //ndef _WIN32
            default: return k;
        }
    }

    // non-blocking getch(); returns 0 if no key was pressed
    inline int nb_getch(void) {
        if(kbhit())
            return getch();
        else
            return 0;
    }

    // returns ANSI color escape sequence for specified color enum
    constexpr str_t getANSIColor(ColorCode c) {
        return
            c==Color_BLACK?ANSI_BLACK:
            c==Color_BLUE?ANSI_BLUE: // non-ANSI
            c==Color_GREEN?ANSI_GREEN:
            c==Color_BOLDGREEN?ANSI_BOLDGREEN:
            c==Color_CYAN?ANSI_CYAN: // non-ANSI
            c==Color_RED?ANSI_RED: // non-ANSI
            c==Color_BOLDRED?ANSI_BOLDRED:
            c==Color_MAGENTA?ANSI_MAGENTA:
            c==Color_BROWN?ANSI_BROWN:
            c==Color_GREY?ANSI_GREY:
            c==Color_DARKGREY?ANSI_DARKGREY:
            c==Color_LIGHTBLUE?ANSI_LIGHTBLUE: // non-ANSI
            c==Color_LIGHTGREEN?ANSI_LIGHTGREEN:
            c==Color_LIGHTCYAN?ANSI_LIGHTCYAN: // non-ANSI;
            c==Color_LIGHTRED?ANSI_LIGHTRED: // non-ANSI;
            c==Color_LIGHTMAGENTA?ANSI_LIGHTMAGENTA:
            c==Color_BOLDYELLOW?ANSI_BOLDYELLOW: // non-ANSI
            c==Color_BOLDWHITE?ANSI_BOLDWHITE:
            c==Color_RESET?ANSI_RESET:
            "";
    }

    // changes color specified by enum (Windows / QBasic colors)
    inline void setColor(ColorCode c) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
        if(c==Color_RESET)
            system("color 07");
        else if(c<Color_RESET) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole,(WORD)c);
        }
#else //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
        lv::safe_print(getANSIColor(c));
#endif //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
    }

    // clears screen and moves cursor home
    inline void cls() {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
        system("cls");
#else //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
        lv::safe_print("\033[2J\033[H");
#endif
    }

    // sets the cursor position to 1-based x,y
    inline void locate(int x, int y) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
        COORD coord;
        coord.X = (SHORT)x-1;
        coord.Y = (SHORT)y-1; // Windows uses 0-based coordinates
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
#else //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
        std::ostringstream oss;
        oss << "\033[" << y << ";" << x << "H";
        lv::safe_print(oss.str());
#endif //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
    }

    // hides the cursor
    inline void hidecursor() {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
        HANDLE hConsoleOutput;
        CONSOLE_CURSOR_INFO structCursorInfo;
        hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleCursorInfo(hConsoleOutput,&structCursorInfo); // Get current cursor size
        structCursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsoleOutput,&structCursorInfo);
#else //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
        lv::safe_print("\033[?25l");
#endif //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
    }

    // shows the cursor
    inline void showcursor() {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
        HANDLE hConsoleOutput;
        CONSOLE_CURSOR_INFO structCursorInfo;
        hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleCursorInfo(hConsoleOutput,&structCursorInfo); // Get current cursor size
        structCursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hConsoleOutput,&structCursorInfo);
#else //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
        lv::safe_print("\033[?25h");
#endif //(!defined(_WIN32) || defined(RLUTIL_USE_ANSI))
    }

    // waits given number of milliseconds before continuing
    inline void msleep(unsigned int ms) {
#ifdef _WIN32
        Sleep(ms);
#else //ndef win32
        // usleep argument must be under 1 000 000
        if(ms > 1000) sleep(ms/1000000);
        usleep((ms % 1000000) * 1000);
#endif //ndef win32
    }

    // get the number of rows in the terminal window, or -1 on error
    inline int trows() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi))
            return -1;
        else
            return csbi.srWindow.Bottom - csbi.srWindow.Top + 1; // Window height
            // return csbi.dwSize.Y; // Buffer height
#else //ndef win32
#ifdef TIOCGSIZE
        struct ttysize ts;
        if(ioctl(STDIN_FILENO,TIOCGSIZE,&ts))
            return -1;
        return ts.ts_lines;
#elif defined(TIOCGWINSZ)
        struct winsize ts;
        if(ioctl(STDIN_FILENO,TIOCGWINSZ,&ts))
            return -1;
        return ts.ws_row;
#else //ndef TIOCGSIZE
        return -1;
#endif //ndef TIOCGSIZE
#endif //ndef _WIN32
    }

    // get the number of columns in the terminal window or -1 on error
    inline int tcols() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi))
            return -1;
        else
            return csbi.srWindow.Right - csbi.srWindow.Left + 1; // Window width
            // return csbi.dwSize.X; // Buffer width
#else //ndef _WIN32
#ifdef TIOCGSIZE
        struct ttysize ts;
        if(ioctl(STDIN_FILENO,TIOCGSIZE,&ts))
            return -1;
        return ts.ts_cols;
#elif defined(TIOCGWINSZ)
        struct winsize ts;
        if(ioctl(STDIN_FILENO,TIOCGWINSZ,&ts))
            return -1;
        return ts.ws_col;
#else //ndef TIOCGSIZE
        return -1;
#endif //ndef TIOCGSIZE
#endif //ndef _WIN32
    }

    // waits until a key is pressed
    inline void anykey() {
        // TODO: Allow optional message for anykey()?
        getch();
    }

} // namespace rlutil

namespace lv {

#if defined(_MSC_VER)
    /// sets the console window to a certain size (with optional buffer resizing)
    inline void setConsoleWindowSize(int x, int y, int buffer_lines=-1) {
        // derived from http://www.cplusplus.com/forum/windows/121444/
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if(h==INVALID_HANDLE_VALUE)
            lvError("setConsoleWindowSize: Unable to get stdout handle");
        COORD largestSize = GetLargestConsoleWindowSize(h);
        if(x>largestSize.X)
            x = largestSize.X;
        if(y>largestSize.Y)
            y = largestSize.Y;
        if(buffer_lines<=0)
            buffer_lines = y;
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        if(!GetConsoleScreenBufferInfo(h,&bufferInfo))
            lvError("setConsoleWindowSize: Unable to retrieve screen buffer info");
        SMALL_RECT& winInfo = bufferInfo.srWindow;
        COORD windowSize = {winInfo.Right-winInfo.Left+1,winInfo.Bottom-winInfo.Top+1};
        if(windowSize.X>x || windowSize.Y>y) {
            SMALL_RECT info = {0,0,SHORT((x<windowSize.X)?(x-1):(windowSize.X-1)),SHORT((y<windowSize.Y)?(y-1):(windowSize.Y-1))};
            if(!SetConsoleWindowInfo(h,TRUE,&info))
                lvError("setConsoleWindowSize: Unable to resize window before resizing buffer");
        }
        COORD size = {SHORT(x),SHORT(y)};
        if(!SetConsoleScreenBufferSize(h,size))
            lvError("setConsoleWindowSize: Unable to resize screen buffer");
        SMALL_RECT info = {0,0,SHORT(x-1),SHORT(y-1)};
        if(!SetConsoleWindowInfo(h, TRUE, &info))
            lvError("setConsoleWindowSize: Unable to resize window after resizing buffer");
    }
#endif //defined(_MSC_VER)

    /// shows a progression bar in the console (20 cols wide & without self-overwrite by default)
    inline void updateConsoleProgressBar(const std::string& sPrefix, float fCompletion, bool bUseCarrRet=false, size_t nBarCols=20) {
        lvDbgAssert_(fCompletion>=0.0f && fCompletion<=1.0f,"completion must be given as fraction");
        lvDbgAssert_(nBarCols>0,"number of columns in progress bar must be positive");
        fCompletion = std::min(std::max(0.0f,fCompletion),1.0f);
        if(nBarCols==0)
            return;
        const int nRows = rlutil::trows();
        const int nCols = rlutil::tcols();
        if(bUseCarrRet)
            printf("\r");
        printf(" %s  ",sPrefix.c_str());
        if(nRows>0 && nCols>0)
            rlutil::setColor(rlutil::Color_BOLDWHITE);
        printf("[");
        const size_t nComplBars = size_t(fCompletion*nBarCols);
        for(size_t n=0; n<nBarCols; ++n) {
            if(nRows>0 && nCols>0) {
                if(n<nBarCols/3)
                    rlutil::setColor(rlutil::Color_BOLDRED);
                else if(n>=(nBarCols-nBarCols/3))
                    rlutil::setColor(rlutil::Color_BOLDGREEN);
                else
                    rlutil::setColor(rlutil::Color_BOLDYELLOW);
            }
            if(n<=nComplBars)
                printf("=");
            else
                printf(" ");
        }
        if(nRows>0 && nCols>0)
            rlutil::setColor(rlutil::Color_BOLDWHITE);
        printf("]");
        if(nRows>0 && nCols>0)
            rlutil::setColor(rlutil::Color_RESET);
        if(bUseCarrRet)
            printf(" ");
        else
            printf("\n");
        fflush(stdout);
    }

    /// cleans a specific row from the console (default=last)
    inline void cleanConsoleRow(int nRowIdx=INT_MAX) {
        if(nRowIdx<0)
            return;
        const int nRows = rlutil::trows();
        const int nCols = rlutil::tcols();
        if(nRows>0 && nCols>0)
            printf("\r%s\r",std::string(nCols,' ').c_str());
        else
            printf("\r");
        fflush(stdout);
    }

    /// progress bar display manager for multi-threaded applications
    struct ProgressBarManager {
        /// initializes the progres bar, & prints initial empty state if init refresh <= 0
        inline ProgressBarManager(const std::string& sPrefix="",
                                  double dInitRefresh_sec=2.0,
                                  double dRefreshRate_sec=0.5,
                                  bool bUseCarrRet=false,
                                  size_t nBarCols=20) :
                m_sBarPrefix(sPrefix),
                m_nBarCols(nBarCols),
                m_bUseCarrRet(bUseCarrRet),
                m_dRefreshRate_sec(dRefreshRate_sec),
                m_dInitRefresh_sec(dInitRefresh_sec) {
            lvAssert_(m_dRefreshRate_sec>0.0,"refresh rate must be positive");
            lvDbgAssert_(m_nBarCols>0,"number of columns in progress bar must be positive");
            m_fLatestCompletion = 0.0f;
            m_nLatestComplBars = size_t(0);
            if(dInitRefresh_sec<=0.0)
                lv::updateConsoleProgressBar(m_sBarPrefix,m_fLatestCompletion,m_bUseCarrRet,m_nBarCols);
        }
        /// updates the progress bar, but only if enough time has elapsed and completion is increasing
        inline bool update(float fCompletion) {
            lvDbgAssert_(fCompletion>=0.0f && fCompletion<=1.0f,"completion must be given as fraction");
            const size_t nComplBars = size_t(fCompletion*m_nBarCols);
            std::lock_guard<std::mutex> oLock(lv::getLogMutex());
            if(fCompletion<=m_fLatestCompletion || nComplBars<=m_nLatestComplBars ||
               (m_nLatestComplBars==size_t(0) && m_oLocalTimer.elapsed()<m_dInitRefresh_sec) ||
               (m_nLatestComplBars>size_t(0) && m_oLocalTimer.elapsed()<m_dRefreshRate_sec))
                return false;
            lv::updateConsoleProgressBar(m_sBarPrefix,fCompletion,m_bUseCarrRet,m_nBarCols);
            m_fLatestCompletion = fCompletion;
            m_nLatestComplBars = nComplBars;
            m_oLocalTimer.tock();
            return true;
        }
        /// reinitializes the progress bar to its default state
        inline void reset() {
            std::lock_guard<std::mutex> oLock(lv::getLogMutex());
            m_fLatestCompletion = 0.0f;
            m_nLatestComplBars = size_t(0);
            lv::updateConsoleProgressBar(m_sBarPrefix,m_fLatestCompletion,m_bUseCarrRet,m_nBarCols);
            m_oLocalTimer.tock();
        }
    protected:
        const std::string m_sBarPrefix;
        const size_t m_nBarCols;
        const bool m_bUseCarrRet;
        const double m_dRefreshRate_sec;
        const double m_dInitRefresh_sec;
        lv::StopWatch m_oLocalTimer;
        float m_fLatestCompletion;
        size_t m_nLatestComplBars;
    };

} // namespace lv
