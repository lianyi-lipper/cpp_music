#pragma once
#include <Windows.h>
#include <thread>
#include <conio.h>
#include <mutex>
#include <regex>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <stdexcept>
#include <limits> // 用于 std::numeric_limits

#define DEBUG 0

class MusicList {
public:
    int dctn = 500;
    std::vector<std::string> vec;

    ~MusicList() {}

    void add(std::string s) {
        vec.push_back(s);
    }

    void clear() {
        vec.clear();
    }

    void setDelay(int _dctn) {
        dctn = _dctn;
    }

    void readFile(std::string fileName = "") {
        clear();
        std::ifstream in(fileName);
        if (!in.is_open()) {
            throw std::runtime_error("Could not open file: " + fileName);
        }

        in >> dctn;
        if (in.fail()) {
            throw std::runtime_error("Failed to read delay value from file: " + fileName);
        }

        // --- 优化点 ---
        // 消耗掉读取 dctn 后剩余的换行符，为 getline 做准备
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string s;
        while (getline(in, s)) {
            // 修正：不再忽略文件中的空行，以支持多重旋律判定
            add(s);
        }
        in.close();
    }

    MusicList(std::string fileName = "") {
        vec.clear();
        if (fileName != "") readFile(fileName);
    }
};

bool isNumeric(std::string const& str) {
    // 修正：允许字符串前后有空格
    try {
        std::string::size_type sz;
        std::stoi(str, &sz);
        // 确保整个字符串都被解析为数字
        return sz == str.length();
    } catch (const std::exception&) {
        return false;
    }
}

class MusicPlayer {
private:
    enum scale {
        Rest = 0,
        C8 = 108,
        B7 = 107, A7s = 106, A7 = 105, G7s = 104, G7 = 103, F7s = 102, F7 = 101, E7 = 100, D7s = 99, D7 = 98, C7s = 97, C7 = 96,
        B6 = 95, A6s = 94, A6 = 93, G6s = 92, G6 = 91, F6s = 90, F6 = 89, E6 = 88, D6s = 87, D6 = 86, C6s = 85, C6 = 84,
        B5 = 83, A5s = 82, A5 = 81, G5s = 80, G5 = 79, F5s = 78, F5 = 77, E5 = 76, D5s = 75, D5 = 74, C5s = 73, C5 = 72,
        B4 = 71, A4s = 70, A4 = 69, G4s = 68, G4 = 67, F4s = 66, F4 = 65, E4 = 64, D4s = 63, D4 = 62, C4s = 61, C4 = 60,
        B3 = 59, A3s = 58, A3 = 57, G3s = 56, G3 = 55, F3s = 54, F3 = 53, E3 = 52, D3s = 51, D3 = 50, C3s = 49, C3 = 48,
        B2 = 47, A2s = 46, A2 = 45, G2s = 44, G2 = 43, F2s = 42, F2 = 41, E2 = 40, D2s = 39, D2 = 38, C2s = 37, C2 = 36,
        B1 = 35, A1s = 34, A1 = 33, G1s = 32, G1 = 31, F1s = 30, F1 = 29, E1 = 28, D1s = 27, D1 = 26, C1s = 25, C1 = 24,
        B0 = 23, A0s = 22, A0 = 21
    };
    const int C_Scale[7][7] = { {C1,D1,E1,F1,G1,A1,B1},
                             {C2,D2,E2,F2,G2,A2,B2},
                             {C3,D3,E3,F3,G3,A3,B3},
                             {C4,D4,E4,F4,G4,A4,B4},
                             {C5,D5,E5,F5,G5,A5,B5},
                             {C6,D6,E6,F6,G6,A6,B6},
                             {C7,D7,E7,F7,G7,A7,B7} };
    const int C_Scale_s[7][7] = { {C1s,D1s,-1,F1s,G1s,A1s,-1},
                                {C2s,D2s,-1,F2s,G2s,A2s,-1},
                                {C3s,D3s,-1,F3s,G3s,A3s,-1},
                                {C4s,D4s,-1,F4s,G4s,A4s,-1},
                                {C5s,D5s,-1,F5s,G5s,A5s,-1},
                                {C6s,D6s,-1,F6s,G6s,A6s,-1},
                                {C7s,D7s,-1,F7s,G7s,A7s,-1} };
    HMIDIOUT handle;
    int dctn = 500;
    int volume = 0x7f;
    static const int BASE_DURATION_UNITS = 672;
public:
    bool ENDMUSIC = 0;
    MusicPlayer() {
        midiOutOpen(&handle, 0, 0, 0, CALLBACK_NULL);
    }
    ~MusicPlayer() {
        midiOutClose(handle);
    }
    void setVolume(int _vol) {
        volume = _vol;
    }
    void setDelay(int _dctn) {
        dctn = _dctn;
    }
    int ttag = 0;
    int tick1, tick2;
    void play_single(std::string s, bool isMain) {
        std::vector <int> nbuf;
        s = s + ' '; int n = s.size();
        int ctn = BASE_DURATION_UNITS, vol = volume;
        bool isChord = 0; nbuf.clear();
        auto st = std::chrono::high_resolution_clock::now();
        int tick = 0;
        for (int i = 0; i < n; ++i) {
            if (ENDMUSIC) break;
            char c = s[i];
            switch (c) {
            case '[':case '{': {
                assert(isChord == 0);
                isChord = 1;
                break;
            }
            case ']':case '}': {
                assert(isChord == 1);
                isChord = 0;
                break;
            }
            case ' ': {
                if (!isChord) {
                    if (!nbuf.empty()) {
                        for (int i = 0; i < (int)nbuf.size(); ++i) if (nbuf[i] != 0) midiOutShortMsg(handle, nbuf[i]); nbuf.clear();
                        double target_ms = (double)dctn / BASE_DURATION_UNITS * (tick + ctn);
                        auto target_time = st + std::chrono::duration<double, std::milli>(target_ms);
                        std::this_thread::sleep_until(target_time);
                        tick += ctn; ctn = BASE_DURATION_UNITS;
                    }
                }
                break;
            }
            case '|':break;
            case '_': {
                ctn /= 2;
                break;
            }
            case '*': {
                ctn /= 3;
                break;
            }
            case '&': {
                ctn /= 7;
                break;
            }
            case '%': {
                ctn /= 5;
                break;
            }
            case '.': {
                ctn *= 1.5;
                break;
            }
            case '-': {
                ctn += BASE_DURATION_UNITS;
                break;
            }
            case '0': {
                nbuf.push_back(Rest);
                break;
            }
            default: {
                if (c >= '1' && c <= '7') {
                    int x = (int)c - 49, lvl = 3;
                    bool isSharp = 0;
                    for (int j = i + 1; j < n; ++j) {
                        if (s[j] == '^') lvl++;
                        else if (s[j] == ',') lvl--;
                        else if (s[j] == '#') isSharp = 1;
                        else break;
                        i++;
                    }
                    if (isSharp) nbuf.push_back((vol << 16) + (C_Scale_s[lvl][x] << 8) + 0x90);
                    else nbuf.push_back((vol << 16) + (C_Scale[lvl][x] << 8) + 0x90);
                }
                break;
            }
            }
        }
        if (isMain) tick1 = tick; else tick2 = tick;
        return;
    }
    void play(std::string s1, std::string s2 = "") {
        tick1 = 0; tick2 = 0;
        std::thread tune1(&MusicPlayer::play_single, this, s1, 1);
        std::thread tune2(&MusicPlayer::play_single, this, s2, 0);
        tune1.join();
        tune2.join();
        if (DEBUG) {
            if (tick1 == tick2) puts("Succ");
            else printf("Warn: %d!=%d\n", tick1, tick2);
        }
    }
    void playList(MusicList& m) {
        dctn = m.dctn; ENDMUSIC = 0;
        for (int i = 0; i < (int)m.vec.size() && !ENDMUSIC; ++i) {
            // 修正：跳过空行，以实现多重旋律的中断
            if (m.vec[i].empty()) {
                continue;
            }

            // 处理数字行（延迟）
            if (isNumeric(m.vec[i])) {
                setDelay(stoi(m.vec[i]));
                continue;
            }

            std::string s1 = m.vec[i], s2 = "";
            // 修正：检查下一行是否是有效的第二轨道（非空且非数字）
            if (i + 1 < (int)m.vec.size() && !m.vec[i + 1].empty() && !isNumeric(m.vec[i + 1])) {
                s2 = m.vec[i + 1];
                i++; // 跳过下一行，因为它已经被用作第二轨道
            }
            play(s1, s2);
        }
    }
};

class BGM {
public:
    MusicPlayer player;
    MusicList nowList;
    std::thread bgm_thread;

    BGM(std::string name, int volume = 0x7f) {
        nowList.readFile(name); player.setVolume(volume);
    }
    ~BGM() {
        if (bgm_thread.joinable()) {
            stop();
        }
    }
    void setMusic(std::string name) {
        nowList.readFile(name);
    }
    void play_thread() {
        while (1) {
            if (player.ENDMUSIC) break;
            player.playList(nowList);
        }
    }
    void play() {
        if (bgm_thread.joinable()) {
            // 已经在播放了，可以打印一个警告或直接返回
            return;
        }
        player.ENDMUSIC = 0;
        bgm_thread = std::thread(&BGM::play_thread, this);
    }
    void stop() {
        player.ENDMUSIC = 1;
        if (bgm_thread.joinable()) {
            bgm_thread.join();
        }
    }
};
