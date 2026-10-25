#include "engine.h"
#include <cstdio>
#include <string>

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    jime::Engine engine;
    if (!engine.Load(argv[1], argv[2])) return 2;
    for (char punctuation : std::string(",.!?")) {
        jime::LiveSession automatic(engine);
        const auto committed = automatic.InputChar(punctuation);
        if (committed.commitText.empty() || !committed.composition.empty()) {
            std::fprintf(stderr, "Default punctuation auto-commit failed: %c\n", punctuation);
            return 1;
        }
        jime::LiveSession editable(engine);
        editable.SetAutoCommitPunctuation(false);
        const auto pending = editable.InputChar(punctuation);
        if (!pending.commitText.empty() || pending.composition.empty()) return 1;
        const auto explicitCommit = editable.CommitAll();
        if (explicitCommit.commitText != committed.commitText || editable.Composing()) return 1;

        editable.InputChar(punctuation);
        const auto erased = editable.Backspace();
        if (!erased.commitText.empty() || editable.Composing()) return 1;
    }
    jime::LiveSession sentence(engine);
    sentence.SetAutoCommitPunctuation(false);
    for (char c : std::string("kyouha,")) {
        if (!sentence.InputChar(c).commitText.empty()) return 1;
    }
    if (sentence.CommitAll().commitText != "今日は、") return 1;
    jime::LiveSession bounded(engine);
    bounded.SetAutoCommitPunctuation(false);
    std::string automaticPrefix;
    std::string input;
    for (int i = 0; i < 10; ++i) input += "kyouhaiitenkidesune.";
    for (char c : input) automaticPrefix += bounded.InputChar(c).commitText;
    if (automaticPrefix.empty() || !bounded.Composing()) return 1;
    std::string expected;
    for (int i = 0; i < 10; ++i) expected += "今日はいい天気ですね。";
    if (automaticPrefix + bounded.CommitAll().commitText != expected) return 1;
    std::puts("Punctuation remains editable when disabled; default auto-commit is preserved.");
    return 0;
}
