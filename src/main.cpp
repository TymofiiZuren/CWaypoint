#include "core.hpp"
#include "learning/lesson.hpp"
#include "text_input.hpp"
#include "ui_layout.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>
using namespace cw;
namespace {
// Charcoal study palette; opaque token pairs are contrast-tested.
const Color paper{16, 19, 24, 255}, ink{232, 237, 245, 255},
    muted{168, 179, 196, 255}, blue{168, 180, 255, 255}, pale{43, 51, 72, 255},
    line{113, 128, 150, 255}, green{117, 214, 160, 255}, red{255, 154, 154, 255},
    surface{25, 30, 38, 255}, raised{36, 44, 56, 255};
enum class Screen {
  Home,
  Catalogue,
  Lesson,
  Progress,
  Review,
  Settings,
  Compare
};
struct UI {
  Font font{}, mono{};
  Screen screen = Screen::Home;
  const std::vector<Lesson> &lessons;
  Store &store;
  Controller &app;
  int lesson = 0, stage = 0, question = 0, walk = 0, compare = 0, hint = 0;
  int focus = 0, widgets = 0, lastWidgets = 0;
  bool tabbing = false, answerFocus = false, resetConfirm = false;
  std::string track = "C", feedback;
  std::map<std::string, TextInput> drafts;
  std::map<std::string, float> catalogueScroll;
  std::map<std::string, int> practicePosition;
  float answerOffset = 0;
  bool selectingAnswer = false;
  int answerWidget = -1;
  TextInput &draft() { return drafts[lessons[lesson].questions[question].id]; }
  Verdict verdict = Verdict::Invalid;
  float scroll = 0, maxScroll = 0, y = 0, x = 0, width = 0;
  std::map<std::string, float> codeOffsets, panelHeights;
  std::string currentScope, automaticScope, copiedCode, copiedText;
  double copiedUntil = 0;
  bool menuOpen = false, menuJustOpened = false, helpOpen = false, keyboardHelp = false;
  bool inContent = false, blockedFresh = false, reviewMode = false;
  std::string reviewTrack;
  std::string automaticAction;
  bool actionUsed = false;
  UI(const std::vector<Lesson> &l, Store &s, Controller &a)
      : lessons(l), store(s), app(a) {
    for (const auto &item : l)
      if (item.id == a.profile.lesson) {
        track = item.track;
        if (a.profile.step == 1)
          practicePosition[item.id] = a.profile.question;
      }
  }
  float size() const { return static_cast<float>(app.profile.textSize); }
  void text(const std::string &s, float px, float py, float sz, Color c = ink,
            bool code = false) {
    DrawTextEx(code ? mono : font, s.c_str(), {px, py}, sz, 0.4f, c);
  }
  float measure(const std::string &s, float sz, bool code = false) {
    return MeasureTextEx(code ? mono : font, s.c_str(), sz, 0.4f).x;
  }
  std::vector<std::string> wrap(const std::string &s, float w, float sz,
                                bool code = false) {
    std::vector<std::string> out;
    std::string current, word;
    auto add = [&]() {
      if (word.empty())
        return;
      std::string next = current.empty() ? word : current + " " + word;
      if (measure(next, sz, code) > w && !current.empty()) {
        out.push_back(current);
        current = word;
      } else
        current = next;
      word.clear();
    };
    for (char c : s) {
      if (c == '\n') {
        add();
        out.push_back(current);
        current.clear();
      } else if (c == ' ') {
        add();
      } else
        word += c;
    }
    add();
    if (!current.empty() || out.empty())
      out.push_back(current);
    return out;
  }
  void paragraph(const std::string &s, Color c = muted, float custom = 0) {
    float sz = custom ? custom : size();
    for (auto &row : wrap(s, width, sz)) {
      text(row, x, y, sz, c);
      y += sz * 1.48f;
    }
    y += 12;
  }
  void title(const std::string &s, const std::string &sub = "") {
    text(s, x, y, 26);
    y += 38;
    if (!sub.empty())
      paragraph(sub);
    y += 8;
  }
  void heading(const std::string &s) {
    for (const auto &row : wrap(s, width, 22)) {
      text(row, x, y, 22);
      y += 32;
    }
    y += 4;
  }
  bool hit(Rectangle r, Vector2 point) {
    if (inContent &&
        (!contentHit(workspaceLayout(GetScreenWidth(), GetScreenHeight()), point.x, point.y) || menuOpen))
      return false;
    return CheckCollisionPointRec(point, r);
  }
  bool visible(Rectangle r) {
    return !inContent || (r.y + r.height > 88 && r.y < GetScreenHeight() - 48);
  }
  bool button(const std::string &label, Rectangle r, bool primary = false,
              bool enabled = true, bool showLabel = true) {
    int id = -1;
    if (enabled)
      id = widgets++;
    bool hot = enabled && visible(r) && hit(r, GetMousePosition());
    bool selected = enabled && id == focus;
    Color bg = primary ? blue : raised;
    if (hot)
      bg = primary ? Color{190, 199, 255, 255} : pale;
    if (!enabled)
      bg = Color{32, 38, 48, 255};
    DrawRectangleRounded(r, 0.16f, 8, bg);
    if (!primary)
      DrawRectangleRoundedLinesEx(r, 0.16f, 8, 1, line);
    if (selected && tabbing && inContent) {
      if (r.y < 108)
        scroll = std::max(0.f, scroll + r.y - 108);
      else if (r.y + r.height > GetScreenHeight() - 70)
        scroll += r.y + r.height - GetScreenHeight() + 70;
    }
    if (selected && tabbing)
      DrawRectangleRoundedLinesEx({r.x - 3, r.y - 3, r.width + 6, r.height + 6},
                                  0.16f, 8, 2, blue);
    float fs = 17;
    while (fs > 13 && measure(label, fs) > r.width - 24)
      fs -= 1;
    if (showLabel) text(label, r.x + 16, r.y + (r.height - fs) / 2, fs,
         !enabled  ? muted
         : primary ? paper
                   : ink);
    bool clicked = hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool key = enabled && !(inContent && menuOpen) && !actionUsed && selected &&
               (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) &&
               !answerFocus;
    bool scripted = enabled && !automaticAction.empty() &&
                    automaticAction == label && !actionUsed &&
                    (automaticScope.empty() || automaticScope == currentScope);
    if (clicked) {
      focus = id;
      tabbing = false;
      answerFocus = false;
    }
    if (clicked || key || scripted) {
      actionUsed = true;
      return true;
    }
    return false;
  }
  bool rowButton(const std::string &s, bool primary = false,
                 bool enabled = true, float w = 0) {
    bool result = button(s, {x, y, w ? w : width, 46}, primary, enabled);
    y += 58;
    return result;
  }
  void go(Screen next) {
    if (screen == Screen::Catalogue)
      catalogueScroll[track] = scroll;
    screen = next;
    menuOpen = false;
    keyboardHelp = false;
    reviewMode = false;
    scroll = next == Screen::Catalogue ? catalogueScroll[track] : 0;
    maxScroll = scroll;
    focus = 0;
    tabbing = false;
    answerFocus = false;
    resetConfirm = false;
  }
  void clearQuestion() {
    helpOpen = false;
    answerOffset = 0;
    selectingAnswer = false;
    feedback.clear();
    hint = 0;
    app.retry();
    answerFocus = false;
  }
  void savePosition() { app.resume(lesson, stage, question, walk); }
  void openLesson(int l, int st = 0, bool restore = false) {
    lesson = l;
    stage = st;
    question = st == 2 ? 2 : st == 1 ? practicePosition[lessons[l].id] : 0;
    walk = 0;
    if (restore && app.profile.lesson == lessons[l].id) {
      question = st == 1 ? app.profile.question : st == 2 ? 2 : 0;
      walk = st == 0 ? app.profile.walkthrough : 0;
    }
    clearQuestion();
    blockedFresh = false;
    go(Screen::Lesson);
    track = lessons[l].track;
    savePosition();
    if (st == 2)
      blockedFresh = !app.issue(l, 2);
  }
  std::string questionLabel(int q) {
    return q == 2 ? "Fresh check" : "Practice " + std::to_string(q + 1);
  }
  void openReview(ReviewItem item) {
    openLesson(item.lesson, item.question == 2 ? 2 : 1);
    question = item.question;
    savePosition();
    reviewMode = true;
  }
  void nextReview() {
    auto items = app.reviewItems(reviewTrack);
    if (items.empty()) {
      go(Screen::Review);
      return;
    }
    auto current =
        std::find_if(items.begin(), items.end(), [&](ReviewItem item) {
          return item.lesson == lesson && item.question == question;
        });
    if (current == items.end())
      openReview(items.front());
    else {
      ++current;
      openReview(current == items.end() ? items.front() : *current);
    }
  }
  void code(const std::string &s, const std::string &key,
            const std::string &caption = "Example code", bool compactCode = false) {
    std::vector<std::string> rows;
    std::string row;
    for (char ch : s) {
      if (ch == '\n') {
        rows.push_back(row);
        row.clear();
      } else
        row += ch;
    }
    rows.push_back(row);
    float widest = 0;
    for (const auto &item : rows)
      widest = std::max(widest, measure(item, size(), true));
    const float maximum = std::max(0.f, widest - width + 32);
    auto &offset = codeOffsets[key];
    offset = std::clamp(offset, 0.f, maximum);
    currentScope = key;
    text(caption, x, y + 10, 16, muted);
    if (!compactCode || maximum > 0) y += 36;
    if (maximum > 0) {
      if (button("Scroll left", {x + width - 374, y, 110, 36}))
        offset = std::max(0.f, offset - 120);
      if (button("Scroll right", {x + width - 252, y, 116, 36}))
        offset = std::min(maximum, offset + 120);
    }
    if (button(copiedCode == key && copiedText == s && GetTime() < copiedUntil
                   ? "Copied"
                   : "Copy code",
               {x + width - 124, y, 124, 36})) {
      SetClipboardText(s.c_str());
      copiedCode = key;
      copiedText = s;
      copiedUntil = GetTime() + 1.5;
    }
    y += 44;
    const float h = rows.size() * (size() * 1.5f) + (compactCode ? 16 : 32);
    Rectangle r{x, y, width, h};
    int id = widgets++;
    if (hit(r, GetMousePosition()) && visible(r) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      focus = id;
      answerFocus = false;
      tabbing = false;
    }
    bool selected = id == focus && !answerFocus && !menuOpen;
    if (selected && tabbing && (r.y < 108 || r.y > GetScreenHeight() - 110))
      scroll = std::max(0.f, scroll + r.y - 108);
    auto repeat = [](int key) {
      return IsKeyPressed(key) || IsKeyPressedRepeat(key);
    };
    if (selected) {
      if (repeat(KEY_LEFT))
        offset = std::max(0.f, offset - 48);
      if (repeat(KEY_RIGHT))
        offset = std::min(maximum, offset + 48);
      if (IsKeyPressed(KEY_HOME))
        offset = 0;
      if (IsKeyPressed(KEY_END))
        offset = maximum;
      if ((IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER) ||
           IsKeyDown(KEY_LEFT_CONTROL)) &&
          IsKeyPressed(KEY_C)) {
        SetClipboardText(s.c_str());
        copiedCode = key;
        copiedText = s;
        copiedUntil = GetTime() + 1.5;
      }
    }
    if (hit(r, GetMousePosition()) &&
        (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
      offset = std::clamp(offset - GetMouseWheelMove() * 30, 0.f, maximum);
    DrawRectangleRounded(r, .08f, 8, raised);
    if (selected)
      DrawRectangleRoundedLinesEx(r, .08f, 8, 2, blue);
    EndScissorMode();
    float top = std::max(y, 88.f),
          bottom = std::min(y + h, static_cast<float>(GetScreenHeight() - 48));
    BeginScissorMode(static_cast<int>(x + 12), static_cast<int>(top),
                     static_cast<int>(width - 24),
                     static_cast<int>(std::max(0.f, bottom - top)));
    float cy = y + (compactCode ? 8 : 16);
    for (const auto &item : rows) {
      text(item, x + 16 - offset, cy, size(), ink, true);
      cy += size() * 1.5f;
    }
    EndScissorMode();
    BeginScissorMode(16, 88, GetScreenWidth() - 32, GetScreenHeight() - 136);
    y += h + 12;
    if (maximum > 0) {
      float trackWidth = width,
            thumb = std::max(24.f, trackWidth * (width - 32) / widest);
      DrawRectangleRec({x, y, width, 4}, line);
      DrawRectangleRec(
          {x + (trackWidth - thumb) * offset / maximum, y, thumb, 4}, blue);
      y += 16;
      paragraph("Focus code for Left / Right and Home / End. Shift + scroll "
                "also moves long lines.",
                muted, 16);
    } else
      y += 8;
    currentScope.clear();
  }
  void input() {
    Rectangle r{x, y, width, 50};
    auto &editor = draft();
    const auto previousText = editor.value();
    int id = widgets++;
    answerWidget = id;
    bool active = id == focus && answerFocus && !menuOpen;
    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    bool command = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    bool word = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT) || control;
    auto repeat = [](int key) {
      return IsKeyPressed(key) || IsKeyPressedRepeat(key);
    };
    auto caretAtMouse = [&]() {
      float px = GetMousePosition().x - x - 16 + answerOffset;
      float before = 0;
      for (size_t i = 0; i < editor.value().size(); ++i) {
        float after = measure(editor.value().substr(0, i + 1), size(), true);
        if (px < (before + after) / 2)
          return i;
        before = after;
      }
      return editor.value().size();
    };
    if (hit(r, GetMousePosition()) && visible(r) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      focus = id;
      answerFocus = true;
      tabbing = false;
      active = true;
      editor.moveTo(caretAtMouse(), shift);
      selectingAnswer = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      selectingAnswer = false;
    if (active && selectingAnswer && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
      editor.moveTo(caretAtMouse(), true);
    if (id == focus && tabbing && !menuOpen) {
      if (r.y < 108)
        scroll = std::max(0.f, scroll + r.y - 108);
      else if (r.y + r.height > GetScreenHeight() - 70)
        scroll += r.y + r.height - GetScreenHeight() + 70;
      answerFocus = true;
      active = true;
    }
    DrawRectangleRounded(r, .12f, 8, raised);
    DrawRectangleRoundedLinesEx(r, .12f, 8, active ? 2 : 1,
                                active ? blue : line);
    if (active) {
      int ch;
      while ((ch = GetCharPressed()) != 0)
        if (!command && !control && ch >= 32 && ch < 127)
          editor.insert(std::string(1, static_cast<char>(ch)));
      if ((command || control) && IsKeyPressed(KEY_Z)) {
        if (shift)
          editor.redo();
        else
          editor.undo();
      }
      if ((command || control) && IsKeyPressed(KEY_A))
        editor.selectAll();
      if ((command || control) && IsKeyPressed(KEY_C) &&
          !editor.selected().empty())
        SetClipboardText(editor.selected().c_str());
      if ((command || control) && IsKeyPressed(KEY_X) &&
          !editor.selected().empty()) {
        SetClipboardText(editor.selected().c_str());
        editor.backspace();
      }
      if ((command || control) && IsKeyPressed(KEY_V)) {
        const char *clip = GetClipboardText();
        if (clip)
          editor.insert(clip);
      }
      if (repeat(KEY_LEFT)) {
        if (command)
          editor.moveTo(0, shift);
        else
          editor.move(-1, shift, word);
      }
      if (repeat(KEY_RIGHT)) {
        if (command)
          editor.moveTo(editor.value().size(), shift);
        else
          editor.move(1, shift, word);
      }
      if (repeat(KEY_HOME))
        editor.moveTo(0, shift);
      if (repeat(KEY_END))
        editor.moveTo(editor.value().size(), shift);
      if (repeat(KEY_BACKSPACE)) {
        if (editor.selected().empty() && (command || word)) {
          if (command)
            editor.moveTo(0, true);
          else
            editor.move(-1, true, true);
        }
        editor.backspace();
      }
      if (repeat(KEY_DELETE))
        editor.eraseForward();
      if (IsKeyPressed(KEY_ENTER)) {
        auto result = app.submit(lesson, question, editor.value());
        feedback = result.message;
        verdict = result.kind;
        answerFocus = false;
      }
    }
    const float caretX =
        measure(editor.value().substr(0, editor.cursor()), size(), true);
    answerOffset = std::max(0.f, std::min(answerOffset, caretX));
    if (caretX - answerOffset > width - 40)
      answerOffset = caretX - width + 40;
    const float top = std::max(y + 6, 100.f),
                bottom = std::min(y + 44,
                                  static_cast<float>(GetScreenHeight() - 48));
    EndScissorMode();
    BeginScissorMode(static_cast<int>(x + 12), static_cast<int>(top),
                     static_cast<int>(width - 24),
                     static_cast<int>(std::max(0.f, bottom - top)));
    if (active && !editor.selected().empty()) {
      float first = measure(editor.value().substr(0, editor.selectionStart()),
                            size(), true);
      float last = measure(editor.value().substr(0, editor.selectionEnd()),
                           size(), true);
      DrawRectangleRec(
          {x + 16 + first - answerOffset, y + 10, last - first, 30}, pale);
    }
    text(editor.value().empty() ? "Type your answer" : editor.value(),
         x + 16 - answerOffset, y + 14, size(),
         editor.value().empty() ? muted : ink, true);
    if (active && static_cast<int>(GetTime() * 2) % 2 == 0)
      DrawRectangle(static_cast<int>(x + 16 + caretX - answerOffset),
                    static_cast<int>(y + 12), 2, 26, blue);
    EndScissorMode();
    BeginScissorMode(16, 88, GetScreenWidth() - 32, GetScreenHeight() - 136);
    y += 62;
    if (editor.value() != previousText && !feedback.empty() &&
        verdict != Verdict::Invalid) {
      feedback = "Draft changed. The previous learning result is unchanged; "
                 "choose Try "
                 "again before checking a new answer.";
      verdict = Verdict::Invalid;
    }
  }
  void editingControls() {
    auto &editor = draft();
    const int inputId = answerWidget;
    bool editAction = false;
    if (button("Undo", {x, y, 100, 36}, false, editor.canUndo())) {
      editor.undo();
      editAction = true;
    }
    if (button("Redo", {x + 112, y, 100, 36}, false, editor.canRedo())) {
      editor.redo();
      editAction = true;
    }
    text("Command+Z / Shift+Command+Z", x + 232, y + 10, 14, muted);
    if (editAction) {
      if (!feedback.empty() && verdict != Verdict::Invalid) {
        feedback = "Draft changed. The previous result is unchanged; choose Try again before checking.";
        verdict = Verdict::Invalid;
      }
      focus = inputId;
      answerFocus = true;
      tabbing = false;
    }
    y += 48;
  }
  void panel(Rectangle r) {
    float roundness = std::min(1.f, 32.f / std::min(r.width, r.height));
    DrawRectangleRounded(r, roundness, 12, surface);
    DrawRectangleRoundedLinesEx(r, roundness, 12, 1, Color{50, 61, 78, 255});
  }
  void logo(float px, float py, float scale = 1) {
    Rectangle r{px, py, 40 * scale, 40 * scale};
    DrawRectangleRounded(r, .35f, 12, blue);
    text("C", px + 9 * scale, py + 7 * scale, 26 * scale, paper);
    DrawCircle(static_cast<int>(px + 31 * scale),
               static_cast<int>(py + 30 * scale), 3 * scale, paper);
  }
  void sidebar() { // The shared navigation entry point now draws the top bar.
    DrawRectangle(0, 0, GetScreenWidth(), 80, surface);
    DrawLine(0, 79, GetScreenWidth(), 79, Color{50, 61, 78, 255});
    if (button("Home", {24, 16, 192, 48}, false, true, false))
      go(Screen::Home);
    DrawRectangleRounded({24,16,192,48}, .25f, 8,
                         screen == Screen::Home ? pale : surface);
    if (tabbing && focus == 0)
      DrawRectangleRoundedLinesEx({24,16,192,48}, .25f, 8, 2, blue);
    logo(28,20);
    text("CWaypoint", 80, 31, 21, ink);
    struct Nav { const char *label; const char *display; Screen target; };
    const Nav nav[] = {{"Lessons", "Lessons", Screen::Catalogue},
      {"Review mistakes", "Review", Screen::Review},
      {"Progress", "Progress", Screen::Progress},
      {"C / C++ comparisons", "Compare", Screen::Compare}};
    float nx = 236;
    for (auto n : nav) {
      bool selected = screen == n.target ||
        (n.target == Screen::Catalogue && screen == Screen::Lesson && !reviewMode) ||
        (n.target == Screen::Review && reviewMode);
      // Keep action identity independent of the shorter navigation label.
      if (button(n.label, {nx, 18, 132, 44}, false, true, false))
        go(n.target);
      DrawRectangleRounded({nx,18,132,44}, .25f, 8, selected ? pale : surface);
      text(n.display, nx + 16, 32, 17, selected ? blue : muted);
      if (selected) DrawRectangle(nx + 16, 65, 34, 2, blue);
      if (tabbing && focus == widgets - 1)
        DrawRectangleRoundedLinesEx({nx,18,132,44}, .25f, 8, 2, blue);
      nx += 140;
    }
    Rectangle gear{static_cast<float>(GetScreenWidth() - 72),18,48,44};
    if (button("Settings", gear, false, true, false)) {
      menuOpen = !menuOpen; menuJustOpened = menuOpen; answerFocus = false;
    }
    DrawRectangleRounded(gear, .3f, 8, menuOpen ? pale : raised);
    Vector2 center{gear.x + 24, gear.y + 22};
    DrawRing(center, 7, 10, 0, 360, 24, ink);
    for (int i = 0; i < 8; ++i) {
      float a = i * PI / 4;
      DrawLineEx({center.x + std::cos(a)*9, center.y + std::sin(a)*9},
                 {center.x + std::cos(a)*14, center.y + std::sin(a)*14}, 3, ink);
    }
    if (tabbing && focus == widgets - 1)
      DrawRectangleRoundedLinesEx(gear, .3f, 8, 2, blue);
    if (CheckCollisionPointRec(GetMousePosition(), gear))
      text("Settings", gear.x - 16, 68, 14, ink);
  }
  void begin() {
    widgets = 0;
    actionUsed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    if (IsKeyPressed(KEY_TAB) && menuOpen) {
      focus = focus == lastWidgets - 2 ? lastWidgets - 1 : lastWidgets - 2;
      tabbing = true;
      answerFocus = false;
    } else if (IsKeyPressed(KEY_TAB)) {
      focus = (focus + (IsKeyDown(KEY_LEFT_SHIFT) ? -1 : 1) +
               std::max(1, lastWidgets)) %
              std::max(1, lastWidgets);
      tabbing = true;
      answerFocus = false;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      if (menuOpen) {
        menuOpen = false;
        focus = 5;
      } else if (resetConfirm)
        resetConfirm = false;
      else if (screen == Screen::Lesson)
        go(reviewMode ? Screen::Review : Screen::Catalogue);
      else
        go(Screen::Home);
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      tabbing = false;
    sidebar();
    if (!menuOpen && !IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT))
      scroll = std::clamp(scroll - GetMouseWheelMove() * 48, 0.f, maxScroll);
    if (!answerFocus) {
      if (IsKeyPressed(KEY_PAGE_DOWN))
        scroll = std::min(maxScroll, scroll + 400);
      if (IsKeyPressed(KEY_PAGE_UP))
        scroll = std::max(0.f, scroll - 400);
    }
    auto layout = workspaceLayout(GetScreenWidth(), GetScreenHeight());
    x = layout.x;
    width = layout.width;
    y = layout.top - scroll;
    panel({x - 16, 92, width + 32, static_cast<float>(GetScreenHeight() - 152)});
    inContent = true;
    BeginScissorMode(16, 88, GetScreenWidth() - 32, GetScreenHeight() - 136);
  }
  void end() {
    maxScroll =
        std::max(0.f, y + scroll - static_cast<float>(GetScreenHeight()) + 82);
    scroll = std::min(scroll, maxScroll);
    EndScissorMode();
    inContent = false;
    if (maxScroll > 0) {
      float h = static_cast<float>(GetScreenHeight() - 170);
      float thumb = std::max(32.f, h * h / (h + maxScroll));
      DrawRectangle(GetScreenWidth() - 10,
                    104 + static_cast<int>((h - thumb) * scroll / maxScroll), 4,
                    static_cast<int>(thumb), line);
    }
    DrawRectangle(0, GetScreenHeight() - 48, GetScreenWidth(), 48, paper);
    const std::string status = app.notice.empty() ? "Offline learning" : app.notice;
    const bool saveProblem = !store.writable() || app.unsavedChanges;
    auto lines = wrap(status, GetScreenWidth() - (saveProblem ? 190.f : 64.f), 14);
    text(saveProblem ? "Progress needs attention. Open save details." : lines[0],
         32, GetScreenHeight() - 31, 14, saveProblem ? red : muted);
    if (saveProblem && button("Save details",
        {static_cast<float>(GetScreenWidth()-156), static_cast<float>(GetScreenHeight()-43),124,38}))
      go(Screen::Settings);
    if (menuOpen) {
      const Rectangle menu{static_cast<float>(GetScreenWidth()-280),80,256,128};
      const bool dismiss = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
          !CheckCollisionPointRec(GetMousePosition(),menu) && !actionUsed;
      panel(menu);
      if (menuJustOpened) { focus = widgets; menuJustOpened = false; }
      if (button("Preferences", {menu.x+12,menu.y+12,232,44})) {
        menuOpen = false;
        go(Screen::Settings);
      }
      if (button("Keyboard help", {menu.x+12,menu.y+68,232,44})) {
        menuOpen = false;
        go(Screen::Settings);
        keyboardHelp = true;
      }
      if (dismiss) { menuOpen = false; focus = 5; }
    }
    lastWidgets = widgets;
    focus = std::clamp(focus, 0, std::max(0, widgets - 1));
  }
  void suggestion(const std::string &selectedTrack, int after = -1) {
    int next = app.nextLesson(selectedTrack, after);
    if (next < 0) {
      paragraph("All available " + selectedTrack +
                " starter practice is complete. You can revisit lessons or "
                "review mistakes; fresh-check evidence stays separate.");
      return;
    }
    heading("Next in " + selectedTrack + ": " + lessons[next].title);
    paragraph(lessons[next].objective);
    if (rowButton("Study " + lessons[next].title, true, true, 320))
      openLesson(next);
  }
  void home() {
    logo(x, y, 1.5f);
    text("CWaypoint", x + 80, y + 4, 34);
    text("Learn C and C++", x + 80, y + 46, 18, muted);
    y += 100;
    if (!app.profile.lesson.empty()) {
      for (int l = 0; l < static_cast<int>(lessons.size()); l++)
        if (lessons[l].id == app.profile.lesson) {
          heading("Pick up where you left off");
          paragraph(lessons[l].track + "  /  " + lessons[l].title);
          const auto &p = app.profile;
          paragraph(p.step == 1 ? "Continue at practice question " +
                                      std::to_string(p.question + 1)
                    : p.step == 2 ? "Continue at the fresh check"
                    : p.step == 3
                        ? "Continue at the lesson summary"
                        : "Continue reading" +
                              (p.walkthrough > 0
                                   ? std::string(" at walkthrough step ") +
                                         std::to_string(p.walkthrough + 1)
                                   : ""),
                    muted, 16);
          if (rowButton("Continue lesson", true, true, 230))
            openLesson(l, app.profile.step, true);
          break;
        }
    } else if (rowButton("Start learning", true, true, 220))
      go(Screen::Catalogue);
    y += 24;
    suggestion(track);
    y += 16;
    heading("A little practice, a clearer idea");
    paragraph("Read an example, predict the result, then check your answer. "
              "Your progress stays on this Mac.");
    if (rowButton("Review mistakes", false, true, 220)) go(Screen::Review);
  }
  void switchTrack(const std::string &next) {
    if (track == next)
      return;
    catalogueScroll[track] = scroll;
    track = next;
    scroll = catalogueScroll[track];
    maxScroll = scroll;
  }
  void catalogue() {
    title("Choose your next concept",
          "C and C++ are separate languages. Choose a track and work through a "
          "small, focused lesson.");
    if (button("C track", {x, y, 140, 44}, track == "C"))
      switchTrack("C");
    if (button("C++ track", {x + 156, y, 140, 44}, track == "C++"))
      switchTrack("C++");
    y += 76;
    for (int i = 0; i < static_cast<int>(lessons.size()); i++)
      if (lessons[i].track == track) {
        heading(lessons[i].title);
        paragraph(lessons[i].objective);
        int done = app.completed(i);
        text(std::to_string(done) + " of 2 practice questions solved", x, y, 16,
             done == 2 ? green : muted);
        y += 30;
        if (rowButton("Open " + lessons[i].title, true, true, 300))
          openLesson(i);
        DrawLine(static_cast<int>(x), static_cast<int>(y + 8),
                 static_cast<int>(x + width), static_cast<int>(y + 8), line);
        y += 36;
      }
    paragraph(
        "Starter content is available now. The remaining curriculum will be "
        "added by Oleksandr; it is not counted as completed or available.",
        muted, 16);
  }
  void lessonScreen() {
    const auto &l = lessons[lesson];
    if (stage == 1)
      practicePosition[l.id] = question;
    title(l.title);
    const std::array<std::string, 4> tabs{"Read", "Practice", "Fresh check",
                                          "Summary"};
    float bw = (width - 24) / 4;
    for (int i = 0; i < 4; i++)
      if (button(tabs[i], {x + i * (bw + 8), y, bw, 42}, stage == i)) {
        reviewMode = false;
        stage = i;
        question = i == 2 ? 2 : i == 1 ? practicePosition[l.id] : 0;
        clearQuestion();
        scroll = 0;
        savePosition();
        blockedFresh = stage == 2 && !app.issue(lesson, 2);
      }
    y += 58;
    if (stage == 0) {
      paragraph(l.body);
      code(l.code, l.id + ":example");
      if (!l.steps.empty() && !validateWalkthrough(l.steps).empty()) {
        heading("Walkthrough unavailable");
        paragraph("This optional walkthrough has an invalid record. The "
                  "explanation and practice questions remain available.");
      } else if (!l.steps.empty()) {
        walk = std::clamp(walk, 0, static_cast<int>(l.steps.size()) - 1);
        heading("Walk through the example");
        paragraph(
            "Predict the next value, then step through the authored snapshots.",
            muted, 16);
        const auto &step = l.steps[walk];
        code(step.code, l.id + ":walk", "Walkthrough code");
        text("Step " + std::to_string(walk + 1) + " of " +
                 std::to_string(l.steps.size()),
             x, y, 16, muted);
        y += 30;
        heading(step.state);
        paragraph(step.explanation);
        if (button("Previous", {x, y, 120, 44}, false, walk > 0)) {
          walk--;
          savePosition();
        }
        if (button("Next step", {x + 132, y, 132, 44}, true,
                   walk + 1 < static_cast<int>(l.steps.size()))) {
          walk++;
          savePosition();
        }
        if (button("Restart", {x + 276, y, 120, 44})) {
          walk = 0;
          savePosition();
        }
        y += 66;
      }
      if (rowButton("Start practice", true, true, 220)) {
        stage = 1;
        question = 0;
        clearQuestion();
        scroll = 0;
        savePosition();
      }
    } else if (stage == 1 || stage == 2) {
      if (blockedFresh) {
        heading("Progress needs attention");
        paragraph(
            "This new fresh check needs a saved issuance record. Practice and "
            "offline hints remain available while saving is unavailable.");
        if (rowButton("Return to practice", true, true, 250)) {
          stage = 1;
          question = 0;
          clearQuestion();
          blockedFresh = false;
          savePosition();
        }
        return;
      }
      if (reviewMode) {
        auto remaining = app.reviewItems(reviewTrack).size();
        paragraph("Mistake review: " + std::to_string(remaining) +
                      " remaining. A successful retry clears this item and "
                      "keeps the original attempt history.",
                  muted, 16);
      }
      if (stage == 1) {
        text("Practice question " + std::to_string(question + 1) + " of 2", x,
             y, 16, muted);
        y += 30;
      } else
        paragraph("This is separate from practice completion. Only the first "
                  "correct attempt without help is recorded as independent.",
                  muted, 16);
      const auto &q = l.questions[question];
      const std::string &questionCode = q.code.empty() ? l.code : q.code;
      const float fullX = x, fullWidth = width, questionTop = y;
      const auto layout = workspaceLayout(GetScreenWidth(), GetScreenHeight());
      float codeBottom = y;
      const std::string panelKey = q.id + ":" + std::to_string(static_cast<int>(width)) + ":" + std::to_string(size());
      if (layout.columns) {
        width = layout.codeWidth - 32;
        panel({x, y, layout.codeWidth, 300});
        x += 16;
        y += 16;
        code(questionCode, l.id + ":example");
        codeBottom = y;
        x = fullX + layout.codeWidth + 24 + 16;
        width = layout.answerWidth - 32;
        y = questionTop + 16;
        panel({x - 16, questionTop, width + 32,
               panelHeights.count(panelKey) ? panelHeights.at(panelKey) : 700.f});
        heading(q.prompt);
      } else {
        heading(q.prompt);
        code(questionCode, l.id + ":example", "Question code", true);
      }
      if (q.type == QuestionType::Choice) {
        for (const auto &choice : q.choices) {
          bool selected = draft().value() == choice;
          if (rowButton((selected ? "Selected: " : "") + choice, selected)) {
            draft().set(choice);
            feedback.clear();
          }
        }
      } else {
        paragraph(q.type == QuestionType::Token
                      ? "Enter the missing token:"
                      : "Enter the output or value (without quotation marks):",
                  muted, 16);
        input();
      }
      if (button("Check answer", {x, y, 180, 46}, verdict == Verdict::Invalid)) {
        auto result = app.submit(lesson, question, draft().value());
        feedback = result.message;
        verdict = result.kind;
        answerFocus = false;
      }
      if (button("Try again", {x + 196, y, 144, 46}, verdict == Verdict::Incorrect)) {
        app.retry();
        feedback.clear();
        answerFocus = q.type != QuestionType::Choice;
      }
      y += 58;
      if (q.type != QuestionType::Choice) editingControls();
      if (!feedback.empty()) {
        paragraph(feedback, verdict == Verdict::Correct     ? green
                            : verdict == Verdict::Incorrect ? red
                                                            : muted);
        paragraph(app.status(lesson, question), muted, 16);
      }
      if (automaticAction == "1. Concept" || automaticAction == "2. Guidance" ||
          automaticAction == "3. Explanation" || automaticAction == "Reveal answer")
        helpOpen = true;
      if (rowButton(helpOpen ? "Hide hints" : "Get a hint", false, true, 180))
        helpOpen = !helpOpen;
      if (helpOpen) {
      paragraph("Opening a hint or revealing the answer counts as help.", muted, 16);
      for (int i = 0; i < 3; i++)
        if (button(std::string(i == 0   ? "1. Concept"
                               : i == 1 ? "2. Guidance"
                                        : "3. Explanation"),
                   {x + i * (width / 3 + 3), y, width / 3 - 8, 44},
                   hint == i + 1)) {
          if (app.help(lesson, question))
            hint = i + 1;
        }
      y += 64;
      if (hint)
        paragraph(q.hints[hint - 1], ink);
      if (rowButton("Reveal answer", false, true, 200)) {
        if (app.help(lesson, question)) {
          hint = 3;
          feedback = "Answer: " + q.answer + ". " + q.explanation;
          verdict = Verdict::Invalid;
        }
      }
      }
      if (reviewMode) {
        auto remaining = app.reviewItems(reviewTrack);
        if (!app.profile.review.count(q.id))
          paragraph("This question is cleared from mistake review.", green);
        if (remaining.empty())
          paragraph("All mistakes in this review filter are cleared. You can "
                    "revisit practice whenever you like.",
                    green);
        if (rowButton("Next review question", true,
                      std::any_of(remaining.begin(), remaining.end(),
                                  [&](ReviewItem item) {
                                    return item.lesson != lesson ||
                                           item.question != question;
                                  }),
                      260))
          nextReview();
        if (rowButton("Back to review list", false, true, 250))
          go(Screen::Review);
      } else if (stage == 1 && question == 0) {
        if (rowButton("Next practice question", app.solved(lesson, question), true, 260)) {
          question = 1;
          clearQuestion();
          scroll = 0;
          savePosition();
        }
      } else if (stage == 1) {
        if (rowButton("Try the fresh check", app.completed(lesson) == 2, true, 240)) {
          stage = 2;
          question = 2;
          clearQuestion();
          scroll = 0;
          savePosition();
          blockedFresh = !app.issue(lesson, 2);
        }
      } else if (rowButton("View summary", true, true, 220)) {
        stage = 3;
        scroll = 0;
        savePosition();
      }
      panelHeights[panelKey] = y - questionTop + 16;
      y = std::max(y, codeBottom) + 16;
      x = fullX;
      width = fullWidth;
    } else {
      heading(app.completed(lesson) == 2 ? "Practice complete"
                                         : "Keep building your understanding");
      paragraph(std::to_string(app.completed(lesson)) +
                " of 2 required practice questions solved. Fresh-check "
                "evidence is reported separately.");
      for (int q = 0; q < 3; q++) {
        heading(q == 2 ? "Fresh check" : "Practice " + std::to_string(q + 1));
        paragraph(app.status(lesson, q), ink);
      }
      if (rowButton("Practise again", true, true, 210)) {
        stage = 1;
        question = 0;
        clearQuestion();
        scroll = 0;
        savePosition();
      }
      if (app.completed(lesson) == 2) {
        y += 16;
        suggestion(l.track, lesson);
      }
      if (rowButton("Choose another lesson", false, true, 260))
        go(Screen::Catalogue);
    }
  }
  void progress() {
    title(
        "Your progress",
        "Practice completion and fresh-check evidence describe different parts "
        "of learning. A single question is not proof of long-term mastery.");
    for (int l = 0; l < static_cast<int>(lessons.size()); l++) {
      heading(lessons[l].track + " / " + lessons[l].title);
      paragraph(std::to_string(app.completed(l)) +
                    " / 2 practice questions solved. Fresh check: " +
                    app.status(l, 2),
                ink);
      for (int q = 0; q < 2; ++q)
        paragraph("Practice " + std::to_string(q + 1) + ": " + app.status(l, q),
                  app.solved(l, q) ? green : muted, 16);
      if (rowButton("Review " + lessons[l].title, false, true, 320))
        openLesson(l, 3);
      y += 16;
    }
    heading("Review mistakes");
    auto remaining = app.reviewItems().size();
    paragraph(remaining ? std::to_string(remaining) +
                              " questions are ready for a focused retry."
                        : "No current questions are waiting for review.");
    if (rowButton("Open mistake review", true, true, 250))
      go(Screen::Review);
  }
  void review() {
    title("Review mistakes",
          "Work through questions you previously answered incorrectly. Correct "
          "retries remove them from this list without rewriting first-attempt "
          "evidence.");
    if (button("All tracks", {x, y, 140, 44}, reviewTrack.empty())) {
      reviewTrack.clear();
      scroll = 0;
      maxScroll = 0;
    }
    if (button("C only", {x + 156, y, 140, 44}, reviewTrack == "C")) {
      reviewTrack = "C";
      scroll = 0;
      maxScroll = 0;
    }
    if (button("C++ only", {x + 312, y, 140, 44}, reviewTrack == "C++")) {
      reviewTrack = "C++";
      scroll = 0;
      maxScroll = 0;
    }
    y += 68;
    auto items = app.reviewItems(reviewTrack);
    if (items.empty()) {
      heading("Nothing waiting here");
      paragraph("There are no current mistakes in this filter. Incorrect "
                "answers will appear here automatically; other tracks may "
                "still have questions to review.");
      if (rowButton("Browse lessons", true, true, 220))
        go(Screen::Catalogue);
      return;
    }
    paragraph(std::to_string(items.size()) + " questions waiting", ink);
    if (rowButton("Start review", true, true, 220))
      openReview(items.front());
    y += 16;
    for (auto item : items) {
      const auto &l = lessons[item.lesson];
      heading(l.track + " / " + l.title + " / " + questionLabel(item.question));
      paragraph(l.questions[item.question].prompt);
      paragraph(app.status(item.lesson, item.question), muted, 16);
      if (rowButton("Review " + l.title + ": " + questionLabel(item.question),
                    false, true, 420))
        openReview(item);
      y += 16;
    }
  }
  void settings() {
    if (keyboardHelp) {
      title("Keyboard shortcuts");
      paragraph("Tab / Shift+Tab: move between controls. Enter or Space: activate. Escape: close the menu or return.");
      paragraph("Answer editing: arrows move the caret; Shift selects; Command+A selects all. Command+C/X/V copy, cut and paste. Command+Z undoes; Shift+Command+Z redoes.");
      paragraph("Page Up / Down scroll the page. Focus a code panel for Left / Right and Home / End, or use Shift + wheel to pan long lines.");
      paragraph("Answers accept single-line ASCII text. Native VoiceOver and Unicode/IME input are not implemented.");
      if (rowButton("Back to preferences", true, true, 240)) keyboardHelp = false;
      return;
    }
    title("Make room to learn",
          "Choose a comfortable reading size. Learning stays offline until a "
          "future AI integration is explicitly configured.");
    heading("Text size");
    paragraph("Current size: " + std::to_string(app.profile.textSize) +
              " px. All lesson text and code use this setting.");
    if (button("Smaller", {x, y, 140, 44}, false, app.profile.textSize > 16))
      app.setTextSize(app.profile.textSize - 2);
    if (button("Larger", {x + 156, y, 140, 44}, false,
               app.profile.textSize < 24))
      app.setTextSize(app.profile.textSize + 2);
    y += 76;
    heading("Optional AI assistance");
    paragraph("Not configured. The application sends no network requests. "
              "Authored hints work at all three levels without credentials.");
    heading("Saved progress");
    paragraph(app.notice.empty() ? store.status() : app.notice, store.writable() ? muted : red);
    if (store.writable() && rowButton("Retry save", false, true, 180)) app.save();
    paragraph(store.writable()
                  ? "This profile is writable. Attempts, help, review items "
                    "and your resume point are saved locally."
                  : "This session cannot write saved progress. Existing files "
                    "are preserved; temporary practice is available.");
    paragraph("Progress folder: " + store.directory().string(), muted, 16);
    heading("Reset learning progress");
    paragraph("Reset starts a new profile generation. Older backup files "
              "remain on disk for preservation and are never automatically "
              "restored into the new generation. This is not secure erasure.");
    if (!resetConfirm) {
      if (rowButton("Reset progress...", false, store.writable(), 220))
        resetConfirm = true;
    } else {
      paragraph("Reset all lesson progress, first-attempt evidence and mistake "
                "review?",
                red);
      if (button("Confirm reset", {x, y, 180, 46}, true)) {
        if (app.reset()) {
          drafts.clear();
          catalogueScroll.clear();
          practicePosition.clear();
          codeOffsets.clear();
        }
        resetConfirm = false;
        clearQuestion();
      }
      if (button("Cancel", {x + 196, y, 120, 46}))
        resetConfirm = false;
      y += 64;
    }
    heading("Keyboard & accessibility");
    paragraph("Use Tab and Shift+Tab to move focus, Enter or Space to activate "
              "controls, and Page Up / Down to scroll. Input accepts plain "
              "text answers; use arrows to move the cursor and Shift to "
              "select. Native VoiceOver access to raylib's custom "
              "controls is not implemented in this build.",
              muted, 16);
  }
  void comparison() {
    auto records = comparisons();
    title("Same goal. Different languages.",
          "Keep each language's rules distinct. These examples show three "
          "common points of comparison.");
    for (int i = 0; i < static_cast<int>(records.size()); i++)
      if (rowButton(records[i].title, compare == i, true, 300))
        compare = i;
    y += 12;
    const auto &r = records[compare];
    auto issue = validateComparison(r);
    if (!issue.empty()) {
      heading("Comparison unavailable");
      paragraph(issue + " Other lessons and comparisons remain available.");
      return;
    }
    heading("C");
    code(r.c, r.title + ":c", "C code");
    heading("C++");
    code(r.cpp, r.title + ":cpp", "C++ code");
    paragraph(r.explanation, ink);
  }
  void draw() {
    begin();
    switch (screen) {
    case Screen::Home:
      home();
      break;
    case Screen::Catalogue:
      catalogue();
      break;
    case Screen::Lesson:
      lessonScreen();
      break;
    case Screen::Progress:
      progress();
      break;
    case Screen::Review:
      review();
      break;
    case Screen::Settings:
      settings();
      break;
    case Screen::Compare:
      comparison();
      break;
    }
    end();
  }
};
std::filesystem::path defaultProfile() {
  const char *home = std::getenv("HOME");
  if (!home)
    throw std::runtime_error("HOME is unavailable; pass --data-dir PATH.");
  return std::filesystem::path(home) / "Library/Application Support/CWaypoint";
}
} // namespace
int main(int argc, char **argv) {
  try {
    std::filesystem::path profile = defaultProfile(), capture;
    bool smoke = false, guided = false, reviewTest = false, codeTest = false,
         undoTest = false, authoredTest = false;
    int frames = 0;
    bool compact = false, largeText = false;
    std::string resumeTest;
    std::string initial = "home";
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "--authored-test") {
        smoke = true;
        authoredTest = true;
        continue;
      }
      if (arg == "--undo-test") {
        smoke = true;
        undoTest = true;
        continue;
      }
      if (arg == "--data-dir" && i + 1 < argc)
        profile = argv[++i];
      else if (arg == "--capture-dir" && i + 1 < argc)
        capture = argv[++i];
      else if (arg == "--screen" && i + 1 < argc)
        initial = argv[++i];
      else if (arg == "--large-text")
        largeText = true;
      else if (arg == "--compact")
        compact = true;
      else if (arg == "--resume-test" && i + 1 < argc) {
        smoke = true;
        resumeTest = argv[++i];
      } else if (arg == "--code-test") {
        smoke = true;
        codeTest = true;
      } else if (arg == "--review-test") {
        smoke = true;
        reviewTest = true;
      } else if (arg == "--guided-test") {
        smoke = true;
        guided = true;
      } else if (arg == "--smoke-test")
        smoke = true;
      else if (arg == "--help") {
        std::cout
            << "CWaypoint [--data-dir PATH] [--capture-dir PATH] [--screen "
               "home|lesson|review|settings|progress|compare] "
               "[--smoke-test|--guided-test|--review-test|--code-test|--undo-test|--authored-test] "
               "[--resume-test "
               "seed|question|walk-seed|walk]\n";
        return 0;
      } else
        throw std::runtime_error("Unknown or incomplete option: " + arg);
    }
    if (!resumeTest.empty() && resumeTest != "seed" &&
        resumeTest != "question" && resumeTest != "walk-seed" &&
        resumeTest != "walk")
      throw std::runtime_error("Unknown resume test phase.");
    if (smoke && profile == defaultProfile())
      throw std::runtime_error("Smoke test requires an isolated --data-dir.");
    if (!capture.empty())
      std::filesystem::create_directories(capture);
    auto lessons = sampleLessons();
    // Isolated guided test verifies that an optional bad record cannot block
    // practice.
    if (guided)
      lessons[2].steps[0].state.clear();
    if (codeTest) {
      lessons[0].code += "\n// " + std::string(140, 'x');
      lessons[0].steps[0].code += " // " + std::string(140, 'y');
    }
    auto error = validate(lessons);
    if (!error.empty())
      throw std::runtime_error(error);
    Store store(profile);
    Controller controller(lessons, store);
    if (reviewTest) {
      controller.submit(0, 0, "incorrect");
      controller.submit(1, 2, "incorrect");
    }
    if (codeTest || largeText)
      controller.setTextSize(24);
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI |
                   FLAG_MSAA_4X_HINT);
    InitWindow(compact ? 960 : 1280, compact ? 640 : 840,
               "CWaypoint - Learn C and C++");
    if (!IsWindowReady())
      throw std::runtime_error("Unable to initialize the application window.");
    SetWindowMinSize(960, 640);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    UI ui(lessons, store, controller);
    ui.font = LoadFontEx("/System/Library/Fonts/Supplemental/Arial.ttf", 48,
                         nullptr, 0);
    ui.mono = LoadFontEx("/System/Library/Fonts/Supplemental/Andale Mono.ttf",
                         40, nullptr, 0);
    bool ownFont = ui.font.texture.id != GetFontDefault().texture.id,
         ownMono = ui.mono.texture.id != GetFontDefault().texture.id;
    if (!ownFont)
      ui.font = GetFontDefault();
    if (!ownMono)
      ui.mono = GetFontDefault();
    SetTextureFilter(ui.font.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(ui.mono.texture, TEXTURE_FILTER_BILINEAR);
    if (initial == "lesson")
      ui.openLesson(0);
    else if (initial == "progress")
      ui.go(Screen::Progress);
    else if (initial == "review")
      ui.go(Screen::Review);
    else if (initial == "settings")
      ui.go(Screen::Settings);
    else if (initial == "compare")
      ui.go(Screen::Compare);
    std::vector<std::string> actions{
        "Start learning", "Open Variables and assignment",
        "Start practice", "Enter answer: 5",
        "Check answer",   "Next practice question",
        "1. Concept",     "Reveal answer",
        "Try again",      "Next practice question"};
    // Smoke exercises the same button handlers as mouse/keyboard; assertions
    // check persisted outcomes.
    if (smoke)
      actions = {"Start learning",
                 "Open Variables and assignment",
                 "Start practice",
                 "Enter answer: 5",
                 "Check answer",
                 "Next practice question",
                 "1. Concept",
                 "Reveal answer",
                 "Read",
                 "Practice",
                 "Home",
                 "Continue lesson",
                 "Try the fresh check",
                 "Reveal answer",
                 "View summary",
                 "Progress",
                 "C / C++ comparisons",
                 "Settings",
                 "Preferences",
                 "Larger",
                 "Settings",
                 "Keyboard help",
                 "Back to preferences",
                 "Home",
                 "Lessons"};
    if (guided)
      actions = {"Study Variables and assignment",
                 "Practice",
                 "Enter answer: 5",
                 "Check answer",
                 "Next practice question",
                 "Check answer",
                 "Try the fresh check",
                 "Check answer",
                 "View summary",
                 "Study Pointers & addresses",
                 "Practice",
                 "&value",
                 "Check answer",
                 "Next practice question",
                 "Check answer",
                 "Summary",
                 "Home",
                 "Progress"};
    if (reviewTest) {
      ui.go(Screen::Review);
      actions = {"C++ only",     "All tracks",
                 "Start review", "Enter answer: 5",
                 "Check answer", "Next review question",
                 "Check answer", "Back to review list",
                 "C++ only",     "Browse lessons",
                 "Progress",     "Review mistakes"};
    }
    if (!resumeTest.empty()) {
      if (resumeTest == "seed")
        actions = {"Study Variables and assignment", "Start practice", "Enter answer: 5",
                   "Check answer", "Next practice question"};
      else if (resumeTest == "walk-seed")
        actions = {"Continue lesson", "Read", "Next step", "Next step"};
      else
        actions = {"Continue lesson"};
    }
    if (codeTest)
      actions = {
          "Study Variables and assignment", "Scroll right", "Scroll right", "Read",
          "Start practice",           "Scroll left",  "Home"};
    if (undoTest)
      actions = {"Study Variables and assignment",
                 "Start practice",
                 "Next practice question",
                 "Undo",
                 "Read",
                 "Practice",
                 "Redo",
                 "Check answer",
                 "Undo",
                 "Check answer",
                 "Try again",
                 "Redo",
                 "Check answer",
                 "Summary"};
    if (authoredTest)
      actions = {"Study Variables and assignment", "Start practice",
                 "Enter answer: wrong", "Check answer", "1. Concept",
                 "Try again", "Enter answer: 5", "Check answer",
                 "Next practice question", "Enter answer: int", "Check answer",
                 "Try the fresh check", "Enter answer: 4", "Check answer",
                 "View summary", "Progress"};
    RenderTexture2D captureTarget{};
    if (!capture.empty())
      captureTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    auto saveCapture = [&](const std::filesystem::path &file) {
      Image image = LoadImageFromTexture(captureTarget.texture);
      ImageFlipVertical(&image);
      ExportImage(image, file.c_str());
      UnloadImage(image);
    };
    size_t action = 0;
    int idle = 0;
    bool testOk = true;
    bool hitTestOk = true;
    bool undoBarrierOk = true;
    if (codeTest) {
      ui.inContent = true;
      hitTestOk =
          !ui.hit({250, 50, 100, 120}, {300, 70}) &&
          ui.hit({250, 50, 100, 120}, {300, 120}) &&
          !ui.hit({250, static_cast<float>(GetScreenHeight() - 80), 100, 80},
                  {300, static_cast<float>(GetScreenHeight() - 30)});
      ui.inContent = false;
    }
    std::filesystem::path pendingCapture;
    int captureWait = 0;
    float expectedCatalogueScroll = 0;
    while (!WindowShouldClose()) {
      if (smoke && frames > 4 && frames % 6 == 0 && action < actions.size())
        ui.automaticAction = actions[action];
      else
        ui.automaticAction.clear();
      if (smoke && !guided && !reviewTest && !codeTest && !undoTest && !authoredTest &&
          resumeTest.empty() && action == 1 && !ui.automaticAction.empty()) {
        ui.scroll = std::min(80.f, ui.maxScroll);
        expectedCatalogueScroll = ui.scroll;
      }
      if (smoke && !guided && !reviewTest && !codeTest && !undoTest && !authoredTest &&
          resumeTest.empty() && action == 6 && !ui.automaticAction.empty()) {
        ui.draft().set("inx");
        ui.draft().moveTo(2);
        ui.draft().eraseForward();
        ui.draft().insert("t");
      }
      if (reviewTest && action == 6 && !ui.automaticAction.empty())
        ui.draft().set("cats");
      if (undoTest && action == 3 && !ui.automaticAction.empty()) {
        ui.draft().set("inx");
        ui.draft().moveTo(2);
        ui.draft().eraseForward();
        ui.draft().insert("t");
      }
      const bool scriptedInput = ui.automaticAction.rfind("Enter answer: ", 0) == 0;
      if (scriptedInput) {
        if (ui.screen != Screen::Lesson || (ui.stage != 1 && ui.stage != 2) ||
            lessons[ui.lesson].questions[ui.question].type == QuestionType::Choice)
          throw std::runtime_error("Scripted answer requires a visible text question.");
        ui.draft().set(ui.automaticAction.substr(14));
      }
      ui.automaticScope.clear();
      if (codeTest) {
        if (action == 1 || action == 5)
          ui.automaticScope = "c-variables:example";
        if (action == 2)
          ui.automaticScope = "c-variables:walk";
      }
      if (guided && !ui.automaticAction.empty()) {
        if (action == 5)
          ui.draft().set("int");
        if (action == 7)
          ui.draft().set("4");
        if (action == 14)
          ui.draft().set("12");
      }
      if (guided && !pendingCapture.empty() &&
          (action == 9 || action == 10 || action == 18))
        ui.scroll = ui.maxScroll;
      if (reviewTest && !pendingCapture.empty() && (action == 5 || action == 7))
        ui.scroll = ui.maxScroll;
      if (resumeTest == "walk" && !pendingCapture.empty())
        ui.scroll = ui.maxScroll;
      if (codeTest && !pendingCapture.empty()) {
        if (action == 2)
          ui.scroll = std::min(ui.maxScroll, 280.f);
        if (action == 3)
          ui.scroll = ui.maxScroll;
      }
      if (undoTest && !pendingCapture.empty() &&
          (action == 4 || action == 7 || action == 10))
        ui.scroll = std::min(ui.maxScroll, 300.f);
      std::filesystem::path captureThisFrame;
      if (captureWait > 0)
        --captureWait;
      else {
        captureThisFrame = pendingCapture;
        pendingCapture.clear();
      }
      if (!capture.empty())
        BeginTextureMode(captureTarget);
      else
        BeginDrawing();
      ClearBackground(paper);
      ui.draw();
      if (scriptedInput)
        ui.actionUsed = true;
      if (!capture.empty()) {
        EndTextureMode();
        BeginDrawing();
        ClearBackground(paper);
        DrawTextureRec(captureTarget.texture,
                       {0, 0, static_cast<float>(captureTarget.texture.width),
                        -static_cast<float>(captureTarget.texture.height)},
                       {0, 0}, WHITE);
        EndDrawing();
      } else
        EndDrawing();
      if (!captureThisFrame.empty())
        saveCapture(captureThisFrame);
      frames++;
      if (smoke && !ui.automaticAction.empty()) {
        if (ui.actionUsed) {
          action++;
          if (undoTest && action == 10)
            undoBarrierOk =
                controller.profile.evidence.at("c-var-p2").attempts == 1;
          idle = 0;
          if (!capture.empty()) {
            captureWait = 2;
            pendingCapture =
                capture / ("step-" + std::to_string(action) + ".png");
          }
        } else if (++idle > 8) {
          std::cerr << "UI action unavailable: " << actions[action] << '\n';
          testOk = false;
          break;
        }
      }
      if (smoke && action == actions.size() && pendingCapture.empty()) {
        if (!guided && !reviewTest && !codeTest && !undoTest && !authoredTest &&
            resumeTest.empty())
          testOk = controller.completed(0) == 1 &&
                   controller.profile.evidence.at("c-var-p2").helped &&
                   controller.profile.evidence.at("c-var-fresh").helped &&
                   controller.profile.textSize == 20 &&
                   ui.lastWidgets == ui.widgets &&
                   ui.drafts.at("c-var-p2").value() == "int" &&
                   ui.practicePosition.at("c-variables") == 1 &&
                   std::abs(ui.scroll - expectedCatalogueScroll) < 1;
        if (authoredTest)
          testOk = lessons[0].body == cwaypoint::variablesLesson().explanation &&
                   lessons[0].questions[0].type == QuestionType::Output &&
                   controller.completed(0) == 2 && controller.reviewItems().empty() &&
                   controller.status(0, 0) == "Solved with practice" &&
                   controller.status(0, 2) == "Solved independently on first attempt" &&
                   controller.profile.evidence.at("c-var-p1").attempts == 2 &&
                   ui.screen == Screen::Progress;
        if (guided)
          testOk = controller.completed(0) == 2 &&
                   controller.completed(2) == 2 &&
                   controller.nextLesson("C") == -1 &&
                   controller.nextLesson("C++") == 1 &&
                   controller.status(0, 2) ==
                       "Solved independently on first attempt" &&
                   ui.screen == Screen::Progress &&
                   !validateWalkthrough(lessons[2].steps).empty();
        if (reviewTest)
          testOk =
              controller.reviewItems().empty() &&
              controller.status(0, 0) == "Solved with practice" &&
              controller.status(1, 2) == "Solved with practice" &&
              controller.profile.evidence.at("c-var-p1").attempts == 2 &&
              controller.profile.evidence.at("cpp-str-fresh").attempts == 2 &&
              ui.screen == Screen::Review;
        if (!resumeTest.empty()) {
          if (resumeTest == "seed") {
            ui.draft().set("unsubmitted draft");
            testOk = controller.profile.step == 1 &&
                     controller.profile.question == 1;
          } else if (resumeTest == "question")
            testOk = ui.lesson == 0 && ui.stage == 1 && ui.question == 1 &&
                     ui.draft().value().empty();
          else if (resumeTest == "walk-seed")
            testOk = controller.profile.step == 0 &&
                     controller.profile.walkthrough == 2;
          else
            testOk = ui.lesson == 0 && ui.stage == 0 && ui.walk == 2;
        }
        if (codeTest)
          testOk = hitTestOk && ui.codeOffsets.at("c-variables:example") == 0 &&
                   ui.codeOffsets.at("c-variables:walk") == 120 &&
                   controller.profile.textSize == 24 &&
                   ui.screen == Screen::Home;
        if (undoTest)
          testOk = undoBarrierOk &&
                   controller.profile.evidence.at("c-var-p2").attempts == 2 &&
                   controller.profile.evidence.at("c-var-p2").independent &&
                   ui.drafts.at("c-var-p2").value() == "int" &&
                   controller.completed(0) == 1 && ui.stage == 3;
        std::cout << (testOk ? "UI journey passed"
                             : "UI journey assertion failed")
                  << '\n';
        break;
      }
      if (!smoke && !capture.empty() && frames == 10) {
        saveCapture(capture / (initial + ".png"));
        break;
      }
    }
    if (!capture.empty())
      UnloadRenderTexture(captureTarget);
    if (ownMono)
      UnloadFont(ui.mono);
    if (ownFont)
      UnloadFont(ui.font);
    CloseWindow();
    return testOk ? 0 : 1;
  } catch (const std::exception &e) {
    std::cerr << "CWaypoint: " << e.what() << '\n';
    return 1;
  }
}
