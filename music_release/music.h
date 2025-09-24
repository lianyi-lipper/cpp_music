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
#include <map>

#define DEBUG 0

class MusicList {
public:
    int dctn = 500;
    int volume = 0x7f; // Default volume
    int instrument = 0; // Default instrument: piano
    std::vector<std::string> vec;
    std::map<std::string, int> instrument_map;

    MusicList(std::string fileName = "") {
        instrument_map["piano"] = 0;
        instrument_map["acousticgrand"] = 0;
        instrument_map["brightacoustic"] = 1;
        instrument_map["electricgrand"] = 2;
        instrument_map["honkytonk"] = 3;
        instrument_map["electricpiano1"] = 4;
        instrument_map["electricpiano2"] = 5;
        instrument_map["harpsichord"] = 6;
        instrument_map["clav"] = 7;
        instrument_map["celesta"] = 8;
        instrument_map["glockenspiel"] = 9;
        instrument_map["musicbox"] = 10;
        instrument_map["vibraphone"] = 11;
        instrument_map["marimba"] = 12;
        instrument_map["xylophone"] = 13;
        instrument_map["tubularbells"] = 14;
        instrument_map["dulcimer"] = 15;
        instrument_map["drawbarorgan"] = 16;
        instrument_map["percussiveorgan"] = 17;
        instrument_map["rockorgan"] = 18;
        instrument_map["churchorgan"] = 19;
        instrument_map["reedorgan"] = 20;
        instrument_map["accordion"] = 21;
        instrument_map["harmonica"] = 22;
        instrument_map["concertina"] = 23;
        instrument_map["guitar"] = 25;
        instrument_map["acousticguitarnylon"] = 24;
        instrument_map["acousticguitarsteel"] = 25;
        instrument_map["electricguitarjazz"] = 26;
        instrument_map["electricguitarclean"] = 27;
        instrument_map["electricguitarmuted"] = 28;
        instrument_map["overdrivenguitar"] = 29;
        instrument_map["distortedguitar"] = 30;
        instrument_map["guitarharmonics"] = 31;
        instrument_map["acousticbass"] = 32;
        instrument_map["electricbassfinger"] = 33;
        instrument_map["electricbasspick"] = 34;
        instrument_map["fretlessbass"] = 35;
        instrument_map["slapbass1"] = 36;
        instrument_map["slapbass2"] = 37;
        instrument_map["synthbass1"] = 38;
        instrument_map["synthbass2"] = 39;
        instrument_map["violin"] = 40;
        instrument_map["viola"] = 41;
        instrument_map["cello"] = 42;
        instrument_map["contrabass"] = 43;
        instrument_map["tremolostrings"] = 44;
        instrument_map["pizzicatostrings"] = 45;
        instrument_map["orchestralharp"] = 46;
        instrument_map["timpani"] = 47;
        instrument_map["stringensemble1"] = 48;
        instrument_map["stringensemble2"] = 49;
        instrument_map["synthstrings1"] = 50;
        instrument_map["synthstrings2"] = 51;
        instrument_map["choiraahs"] = 52;
        instrument_map["voiceoohs"] = 53;
        instrument_map["synthvoice"] = 54;
        instrument_map["orchestrahit"] = 55;
        instrument_map["trumpet"] = 56;
        instrument_map["trombone"] = 57;
        instrument_map["tuba"] = 58;
        instrument_map["mutedtrumpet"] = 59;
        instrument_map["frenchhorn"] = 60;
        instrument_map["brasssection"] = 61;
        instrument_map["synthbrass1"] = 62;
        instrument_map["synthbrass2"] = 63;
        instrument_map["sopranosax"] = 64;
        instrument_map["altosax"] = 65;
        instrument_map["tenorsax"] = 66;
        instrument_map["baritonesax"] = 67;
        instrument_map["oboe"] = 68;
        instrument_map["englishhorn"] = 69;
        instrument_map["bassoon"] = 70;
        instrument_map["clarinet"] = 71;
        instrument_map["piccolo"] = 72;
        instrument_map["flute"] = 73;
        instrument_map["recorder"] = 74;
        instrument_map["panflute"] = 75;
        instrument_map["blownbottle"] = 76;
        instrument_map["shakuhachi"] = 77;
        instrument_map["whistle"] = 78;
        instrument_map["ocarina"] = 79;
        instrument_map["lead1square"] = 80;
        instrument_map["lead2sawtooth"] = 81;
        instrument_map["lead3calliope"] = 82;
        instrument_map["lead4chiff"] = 83;
        instrument_map["lead5charang"] = 84;
        instrument_map["lead6voice"] = 85;
        instrument_map["lead7fifths"] = 86;
        instrument_map["lead8basslead"] = 87;
        instrument_map["pad1newage"] = 88;
        instrument_map["pad2warm"] = 89;
        instrument_map["pad3polysynth"] = 90;
        instrument_map["pad4choir"] = 91;
        instrument_map["pad5bowed"] = 92;
        instrument_map["pad6metallic"] = 93;
        instrument_map["pad7halo"] = 94;
        instrument_map["pad8sweep"] = 95;
        instrument_map["fx1rain"] = 96;

        vec.clear();
        if (fileName != "") readFile(fileName);
    }

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

        std::string line;
        std::regex settings_regex(R"(\s*([a-zA-Z]+)\s*=\s*([a-zA-Z0-9]+)\s*)");
        std::smatch match;

        while (getline(in, line)) {
            // Trim whitespace from line
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);
            if (line.empty()) continue;

            if (std::regex_match(line, match, settings_regex)) {
                std::string key = match[1];
                std::string value_str = match[2];
                if (key == "v" || key == "volume") {
                    volume = std::stoi(value_str);
                } else if (key == "dctn" || key == "delay") {
                    dctn = std::stoi(value_str);
                } else if (key == "instrument") {
                    std::string instrument_name = value_str;
                    std::transform(instrument_name.begin(), instrument_name.end(), instrument_name.begin(), ::tolower);
                    if (instrument_map.count(instrument_name)) {
                        instrument = instrument_map[instrument_name];
                    }
                }
            } else {
                // First non-setting line
                add(line);
                break;
            }
        }

        while (getline(in, line)) {
            add(line);
        }
        in.close();
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
    int instrument = 0;
    int channel = 0;
    std::map<std::string, int> instrument_map;
    static const int BASE_DURATION_UNITS = 672;
public:
    bool ENDMUSIC = 0;
    MusicPlayer() {
        midiOutOpen(&handle, 0, 0, 0, CALLBACK_NULL);
        setInstrument(instrument);
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
    void setInstrument(int instrument_id) {
        instrument = instrument_id;
        // Program Change: 0xC0 | channel, instrument
        midiOutShortMsg(handle, (instrument << 8) | (0xC0 | channel));
    }
    int ttag = 0;
    int tick1, tick2;
    void play_single(std::string s, bool isMain) {
        std::vector <int> nbuf;
        s = s + ' '; int n = s.size();
        int ctn = BASE_DURATION_UNITS;
        int current_vol = volume;
        bool isChord = 0; nbuf.clear();
        auto st = std::chrono::high_resolution_clock::now();
        int tick = 0;
        for (int i = 0; i < n; ++i) {
            if (ENDMUSIC) break;
            char c = s[i];
            switch (c) {
            case '[':case '{': {
                if (s.substr(i, 12) == "[instrument=") {
                    int j = i + 12;
                    std::string instrument_name = "";
                    while (j < n && s[j] != ']') {
                        instrument_name += s[j];
                        j++;
                    }
                    if (j < n) { // found closing ']'
                        std::transform(instrument_name.begin(), instrument_name.end(), instrument_name.begin(), ::tolower);
                        if (instrument_map.count(instrument_name)) {
                            setInstrument(instrument_map[instrument_name]);
                        }
                        i = j; // Move past the instrument command
                    }
                } else {
                    assert(isChord == 0);
                    isChord = 1;
                }
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
            case 'v': {
                int vel = 0;
                int j = i + 1;
                while (j < n && isdigit(s[j])) {
                    vel = vel * 10 + (s[j] - '0');
                    j++;
                }
                if (j > i + 1) {
                    current_vol = std::min(127, std::max(0, vel));
                    i = j - 1;
                }
                break;
            }
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
                    int temp_vol = -1;

                    int j = i + 1;
                    while (j < n) {
                        if (s[j] == '^') lvl++;
                        else if (s[j] == ',') lvl--;
                        else if (s[j] == '#') isSharp = 1;
                        else if (s[j] == 'v') {
                            int vel = 0;
                            int k = j + 1;
                            while (k < n && isdigit(s[k])) {
                                vel = vel * 10 + (s[k] - '0');
                                k++;
                            }
                            if (k > j + 1) {
                                temp_vol = std::min(127, std::max(0, vel));
                                j = k - 1;
                            }
                        }
                        else break;
                        j++;
                    }
                    i = j -1;

                    int final_vol = (temp_vol != -1) ? temp_vol : current_vol;

                    if (isSharp) nbuf.push_back((final_vol << 16) + (C_Scale_s[lvl][x] << 8) + 0x90);
                    else nbuf.push_back((final_vol << 16) + (C_Scale[lvl][x] << 8) + 0x90);
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
        dctn = m.dctn;
        volume = m.volume;
        instrument = m.instrument;
        instrument_map = m.instrument_map;
        setInstrument(instrument);
        ENDMUSIC = 0;
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
