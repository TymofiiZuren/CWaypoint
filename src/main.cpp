#include "core.hpp"
#include "report.hpp"
#include "streak_icon.hpp"
#include "learning/lesson.hpp"
#include "text_input.hpp"
#include "keyboard_input.hpp"
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
enum class ButtonStyle { Standard, Quiet, Destructive };
struct UI {
  Font font{}, mono{};
  Texture2D streakFire{}, streakInactive{};
  Screen screen = Screen::Home;
  const std::vector<Lesson> &lessons;
  Store &store;
  Controller &app;
  int lesson = 0, stage = 0, question = 0, walk = 0, compare = 0, hint = 0;
  int focus = 0, widgets = 0, lastWidgets = 0;
  KeyboardModifiers modifiers{};
  bool windowFocused = true;
  bool tabbing = false, answerFocus = false, resetConfirm = false;
  std::string track = "C", feedback, exportNotice;
  bool exportFailed = false;
  std::map<std::string, TextInput> drafts;
  std::map<std::string, float> catalogueScroll;
  std::map<std::string, int> practicePosition;
  float answerOffset = 0;
  bool selectingAnswer = false;
  int answerWidget = -1;
  TextInput chatDraft;
  bool chatFocus = false;
  std::map<std::string, std::pair<std::string, std::string>> conversations;
  TextInput &draft() { return drafts[lessons[lesson].questions[question].id]; }
  Verdict verdict = Verdict::Invalid;
  float scroll = 0, maxScroll = 0, y = 0, x = 0, width = 0;
  std::map<std::string, float> codeOffsets, assistantScroll;
  std::string currentScope, automaticScope, copiedCode, copiedText;
  double copiedUntil = 0;
  bool revealModule = true, moduleBody = false;
  float moduleTop = 0, moduleBottom = 0;
  std::map<std::string, float> moduleScroll;
  bool inCoursePanels = false;
  std::string assistantKey;
  bool assistantBody = false;
  float assistantTop = 0, assistantBottom = 0;
  bool fontMenuOpen = false, drawingFontMenu = false, fontMenuJustOpened = false;
  Rectangle fontField{};
  int fontFieldId = 0, fontOption = 1;
  bool menuOpen = false, menuJustOpened = false, keyboardHelp = false;
  bool inContent = false, blockedFresh = false, reviewMode = false;
  std::string reviewTrack;
  std::string automaticAction;
  bool actionUsed = false;
  bool pointerHitTest = false;
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
    if(fontMenuOpen && !drawingFontMenu) return false;
    if(menuOpen && inCoursePanels) return false;
    if(assistantBody && (point.y < assistantTop || point.y >= assistantBottom)) return false;
    if(moduleBody && (point.y < moduleTop || point.y >= moduleBottom)) return false;
    if (inContent) {
      if (menuOpen) return false;
      const bool inViewport = screen == Screen::Lesson
          ? courseMaterialHit(courseLayout(GetScreenWidth()), GetScreenHeight(), point.x, point.y)
          : contentHit(workspaceLayout(GetScreenWidth(), GetScreenHeight()), point.x, point.y);
      if (!inViewport) return false;
    }
    return CheckCollisionPointRec(point, r);
  }
  bool visible(Rectangle r) {
    return !inContent || (r.y + r.height > 88 && r.y < GetScreenHeight() - 48);
  }
  bool button(const std::string &label, Rectangle r, bool primary = false,
              bool enabled = true, bool showLabel = true,
              ButtonStyle style = ButtonStyle::Standard) {
    int id = -1;
    if (enabled)
      id = widgets++;
    const bool injectedPointer = pointerHitTest && automaticAction == label && !actionUsed;
    const Vector2 pointer = injectedPointer ? Vector2{r.x+r.width/2,r.y+r.height/2} : GetMousePosition();
    bool hot = enabled && visible(r) && hit(r, pointer);
    bool selected = enabled && id == focus;
    const bool quiet = style == ButtonStyle::Quiet || !showLabel;
    const bool destructive = style == ButtonStyle::Destructive;
    const bool pressed = hot && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    Color bg = primary ? blue : raised;
    if (destructive) bg = primary ? red : Color{49, 32, 38, 255};
    if (hot) bg = destructive ? Color{85, 44, 52, 255}
                             : primary ? Color{190, 199, 255, 255} : Color{45, 55, 70, 255};
    if (pressed) bg = destructive ? Color{104, 52, 61, 255}
                                 : primary ? Color{145, 158, 239, 255} : pale;
    if (!enabled) bg = Color{30, 35, 44, 255};
    const float radius = std::min(1.f, 16.f / std::min(r.width, r.height));
    if (!quiet || hot) DrawRectangleRounded(r, radius, 12, bg);
    if (!quiet && !primary)
      DrawRectangleRoundedLinesEx(r, radius, 12, 1,
          !enabled ? Color{48, 55, 66, 255} : destructive ? red : line);
    if (hot) SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    if(selected && tabbing && moduleBody) {
      auto &position=moduleScroll[lessons[lesson].track];
      if(r.y<moduleTop) position=std::max(0.f,position+r.y-moduleTop);
      else if(r.y+r.height>moduleBottom) position+=r.y+r.height-moduleBottom;
    }
    if(selected && tabbing && assistantBody) {
      auto &position=assistantScroll[assistantKey];
      if(r.y < assistantTop) position=std::max(0.f,position+r.y-assistantTop);
      else if(r.y+r.height>assistantBottom) position+=r.y+r.height-assistantBottom;
    }
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
         : destructive ? (primary && !hot ? paper : red)
         : primary ? paper
                   : ink);
    bool clicked = !actionUsed && hot && (injectedPointer || IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
    bool key = windowFocused && enabled && (!fontMenuOpen || drawingFontMenu) && !((inContent || inCoursePanels) && menuOpen) && !actionUsed && selected &&
               (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) &&
               !answerFocus;
    bool scripted = (!fontMenuOpen || drawingFontMenu) && !pointerHitTest && enabled && !automaticAction.empty() &&
                    automaticAction == label && !actionUsed &&
                    (automaticScope.empty() || automaticScope == currentScope);
    if (clicked) {
      focus = id;
      tabbing = false;
      answerFocus = false;
      chatFocus = false;
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
    chatFocus = false;
    if (screen == Screen::Catalogue)
      catalogueScroll[track] = scroll;
    screen = next;
    menuOpen = false;
    fontMenuOpen = false;
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
    chatFocus = false;
    answerOffset = 0;
    selectingAnswer = false;
    feedback.clear();
    verdict = Verdict::Invalid;
    hint = 0;
    app.retry();
    answerFocus = false;
  }
  void savePosition() { app.resume(lesson, stage, question, walk); }
  void openLesson(int l, int st = 0, bool restore = false) {
    revealModule = true;
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
      if ((modifiers.command || modifiers.control) &&
          IsKeyPressed(KEY_C)) {
        SetClipboardText(s.c_str());
        copiedCode = key;
        copiedText = s;
        copiedUntil = GetTime() + 1.5;
      }
    }
    if (hit(r, GetMousePosition()) &&
        modifiers.shift)
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
    contentClip();
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
    bool active = id == focus && answerFocus && !menuOpen && !chatFocus && windowFocused;
    bool shift = modifiers.shift;
    bool command = modifiers.command;
    bool control = modifiers.control;
    bool word = modifiers.alt || control;
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
      chatFocus = false;
      tabbing = false;
      active = windowFocused;
      editor.moveTo(caretAtMouse(), shift);
      selectingAnswer = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      selectingAnswer = false;
    if (active && selectingAnswer && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
      editor.moveTo(caretAtMouse(), true);
    if (id == focus && tabbing && !menuOpen && windowFocused) {
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
        if (acceptTextCharacter(ch, modifiers))
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
    contentClip();
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
  void contentClip() {
    if(screen == Screen::Lesson && inContent) {
      auto c = courseLayout(GetScreenWidth());
      BeginScissorMode(static_cast<int>(c.materialX),88,
                       static_cast<int>(c.materialWidth),GetScreenHeight()-136);
    } else BeginScissorMode(16,88,GetScreenWidth()-32,GetScreenHeight()-136);
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
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
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
    const Nav nav[] = {{"Lessons", "Courses", Screen::Catalogue},
      {"Review mistakes", "Review", Screen::Review},
      {"Progress", "Progress", Screen::Progress},
      {"C / C++ comparisons", "Compare", Screen::Compare}};
    float nx = 236;
    for (auto n : nav) {
      bool selected = screen == n.target ||
        (n.target == Screen::Catalogue && screen == Screen::Lesson && !reviewMode) ||
        (n.target == Screen::Review && reviewMode);
      // Keep action identity independent of the shorter navigation label.
      if (button(n.label, {nx, 18, 116, 44}, false, true, false))
        go(n.target);
      DrawRectangleRounded({nx,18,116,44}, .25f, 8, selected ? pale : surface);
      text(n.display, nx + 16, 32, 17, selected ? blue : muted);
      if (selected) DrawRectangle(nx + 16, 65, 34, 2, blue);
      if (tabbing && focus == widgets - 1)
        DrawRectangleRoundedLinesEx({nx,18,116,44}, .25f, 8, 2, blue);
      nx += 124;
    }
    const int days = studyStreak(app.profile.studyDays, localStudyDay());
    const int level = days >= 14 ? 4 : days >= 7 ? 3 : days >= 3 ? 2 : 1;
    const float badgeX = GetScreenWidth() - 184;
    const Rectangle badge{badgeX, 20, 88, 40};
    DrawRectangleRounded(badge, .3f, 12, raised);
    const float iconSize = 24.f + (days ? level : 0);
    if (days)
      DrawCircleGradient({badgeX + 24, 40}, 15 + level,
                         Fade(ORANGE, .08f * level), Fade(ORANGE, 0));
    DrawTexturePro(days ? streakFire : streakInactive, {0, 0, 72, 72},
                   {badgeX + 10, 40 - iconSize / 2, iconSize, iconSize},
                   {0, 0}, 0, WHITE);
    text(std::to_string(days), badgeX + 49, 30, 20, days ? ink : muted);
    if (CheckCollisionPointRec(GetMousePosition(), badge)) {
      DrawRectangle(GetScreenWidth()-370,72,346,32,raised);
      text(std::to_string(days) + "-day streak. Solve one question today.",
           GetScreenWidth()-360,80,14,ink);
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
    windowFocused = IsWindowFocused();
#ifdef __APPLE__
    modifiers = currentKeyboardModifiers();
#else
    modifiers = {
      IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT),
      IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER),
      IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL),
      IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)
    };
#endif
    if (!windowFocused) selectingAnswer = false;
    widgets = 0;
    actionUsed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    if (IsKeyPressed(KEY_TAB)) chatFocus = false;
    bool closedFontMenu = false;
    if (fontMenuOpen && (IsKeyPressed(KEY_ESCAPE) || GetMouseWheelMove() != 0 ||
        IsKeyPressed(KEY_PAGE_DOWN) || IsKeyPressed(KEY_PAGE_UP))) {
      fontMenuOpen = false; closedFontMenu = true; focus = fontFieldId;
      actionUsed = true;
    }
    if (fontMenuOpen && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_TAB))) {
      fontOption = (fontOption + ((IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && modifiers.shift)) ? 4 : 1)) % 5;
      tabbing = true;
    } else if (IsKeyPressed(KEY_TAB) && menuOpen) {
      focus = focus == lastWidgets - 2 ? lastWidgets - 1 : lastWidgets - 2;
      tabbing = true;
      answerFocus = false;
    } else if (IsKeyPressed(KEY_TAB)) {
      focus = (focus + (modifiers.shift ? -1 : 1) +
               std::max(1, lastWidgets)) %
              std::max(1, lastWidgets);
      tabbing = true;
      answerFocus = false;
    }
    if (IsKeyPressed(KEY_ESCAPE) && !closedFontMenu) {
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
    if (!menuOpen && (screen != Screen::Lesson || (GetMouseX() >= courseLayout(GetScreenWidth()).materialX && GetMouseX() < courseLayout(GetScreenWidth()).assistantX)) && !modifiers.shift)
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
    if(screen==Screen::Lesson) {
      auto course=courseLayout(GetScreenWidth());
      x=course.materialX+16; width=course.materialWidth-32;
    }
    panel({x - 16, 92, width + 32, static_cast<float>(GetScreenHeight() - 152)});
    inContent = true;
    contentClip();
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
    if (fontMenuOpen) {
      const float popupY = fontField.y + fontField.height + 8;
      Rectangle popup{fontField.x, std::min(popupY, static_cast<float>(GetScreenHeight()-316)), fontField.width, 256};
      popup.y = std::max(88.f, popup.y);
      const bool outside = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !actionUsed &&
          !CheckCollisionPointRec(GetMousePosition(), popup) &&
          !CheckCollisionPointRec(GetMousePosition(), fontField);
      if (outside) { fontMenuOpen = false; actionUsed = true; focus = fontFieldId; }
      else {
        panel(popup);
        drawingFontMenu = true;
        // Only the popup options participate in keyboard selection while open.
        if (tabbing || fontMenuJustOpened) focus = widgets + fontOption;
        fontMenuJustOpened = false;
        for (int i=0; i<5; ++i) {
          const int value = 16 + i*2;
          Rectangle option{popup.x+8,popup.y+8+i*48.f,popup.width-16,48};
          const bool current = app.profile.textSize==value;
          if (current) DrawRectangleRounded(option,.25f,12,pale);
          if (button(std::to_string(value)+" px",option,false,true,true,ButtonStyle::Quiet)) {
            app.setTextSize(value);
            fontMenuOpen = false; focus = fontFieldId;
          }
          if (current) {
            DrawLineEx({option.x+option.width-28,option.y+24},{option.x+option.width-23,option.y+29},2,blue);
            DrawLineEx({option.x+option.width-23,option.y+29},{option.x+option.width-14,option.y+18},2,blue);
          }
        }
        drawingFontMenu = false;
      }
    }
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
  void home() { catalogue(); }
  void switchTrack(const std::string &next) {
    if (track == next)
      return;
    catalogueScroll[track] = scroll;
    track = next;
    scroll = catalogueScroll[track];
    maxScroll = scroll;
  }
  void catalogue() {
    title("Your courses", "Choose a language. Learn one concept at a time.");
    if (!app.profile.lesson.empty()) {
      for (int i = 0; i < static_cast<int>(lessons.size()); ++i)
        if (lessons[i].id == app.profile.lesson) {
          paragraph("Continue: " + lessons[i].title, ink, 16);
          if (rowButton("Continue lesson", true, true, 220))
            openLesson(i, app.profile.step, true);
        }
    }
    const float startX = x, fullWidth = width, top = y;
    const float cardWidth = (width - 24) / 2;
    float bottom = y;
    for (int c = 0; c < 2; ++c) {
      const std::string language = c == 0 ? "C" : "C++";
      x = startX + c * (cardWidth + 24);
      y = top;
      panel({x, y, cardWidth, 380});
      DrawRectangleRounded({x+1,y+1,cardWidth-2,100}, .18f, 12,
                           c == 0 ? Color{39,52,76,255} : Color{49,43,73,255});
      text(language, x+24, y+28, 36, blue);
      x += 24; y += 124; width = cardWidth - 48;
      heading(c == 0 ? "C foundations" : "C++ foundations");
      paragraph(c == 0 ? "Values, memory and the building blocks of C."
                       : "Objects, strings and the language of modern C++.", muted, 16);
      int count = 0, complete = 0, first = -1;
      for (int i = 0; i < static_cast<int>(lessons.size()); ++i)
        if (lessons[i].track == language) {
          if (first < 0) first = i;
          ++count; complete += app.completed(i) == 2;
        }
      paragraph(std::to_string(count) + (count == 1 ? " available module  /  " : " available modules  /  ") +
                std::to_string(complete) + " complete", muted, 16);
      if (rowButton("Open " + language + " course", true, first >= 0, 220) ||
          (!actionUsed && !menuOpen && first>=0 &&
           hit({startX+c*(cardWidth+24),top,cardWidth,380},GetMousePosition()) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        int next = app.nextLesson(language);
        actionUsed=true;
        track = language;
        openLesson(next >= 0 ? next : first);
      }
      bottom = std::max(bottom, top + 404);
    }
    x = startX; width = fullWidth; y = bottom;
    paragraph("Your progress is saved locally. More modules will appear as they become available.", muted, 16);
  }
  void coursePanels() {
    const auto geometry = courseLayout(GetScreenWidth());
    const float centerX = x, centerY = y, centerWidth = width;
    const bool wasContent = inContent;
    inContent = false;
    inCoursePanels = true;
    EndScissorMode();
    contentClip();
    const auto &current = lessons[lesson];
    panel({geometry.moduleX,96,geometry.moduleWidth,
           static_cast<float>(GetScreenHeight()-156)});
    x = geometry.moduleX+12; y = 116; width = geometry.moduleWidth-24;
    text(current.track + " COURSE", x, y, 14, blue); y += 30;
    heading("Modules");
    moduleTop=y; moduleBottom=GetScreenHeight()-124.f;
    auto &modulePosition=moduleScroll[current.track];
    if(!menuOpen && GetMouseX()>=geometry.moduleX && GetMouseX()<geometry.moduleX+geometry.moduleWidth &&
       GetMouseY()>=moduleTop && GetMouseY()<moduleBottom)
      modulePosition=std::max(0.f,modulePosition-GetMouseWheelMove()*40);
    EndScissorMode();
    BeginScissorMode(static_cast<int>(x),static_cast<int>(moduleTop),static_cast<int>(width),
                     static_cast<int>(moduleBottom-moduleTop));
    moduleBody=true; y-=modulePosition;
    int number = 0;
    for (int i=0; i<static_cast<int>(lessons.size()); ++i) {
      if(lessons[i].track != current.track) continue;
      ++number;
      const std::string key = "Module: " + lessons[i].title;
      auto rows = wrap(lessons[i].title,width-20,16);
      float height = 40 + rows.size()*22;
      Rectangle r{x,y,width,height};
      bool selected = i == lesson;
      if(selected && revealModule) {
        if(r.y<moduleTop) modulePosition=std::max(0.f,modulePosition+r.y-moduleTop);
        else if(r.y+r.height>moduleBottom) modulePosition+=r.y+r.height-moduleBottom;
        revealModule=false;
      }
      if(button(key,r,false,true,false)) { openLesson(i); }
      if(selected) DrawRectangleRounded(r,.15f,8,pale);
      if(selected) DrawRectangleRec({x,y+8,3,height-16},blue);
      text("MODULE " + std::to_string(number),x+10,y+10,12,muted);
      float ty = y+32;
      for(const auto &row:rows) { text(row,x+10,ty,16,selected?ink:muted); ty+=22; }
      if(tabbing && focus==widgets-1) DrawRectangleRoundedLinesEx(r,.15f,8,2,blue);
      y += height+12;
      text(std::to_string(app.completed(i)) + " / 2 practice",x+10,y-8,13,muted);
      y += 24;
    }
    modulePosition=std::min(modulePosition,std::max(0.f,y+modulePosition-moduleBottom));
    moduleBody=false;
    EndScissorMode(); contentClip();
    y=GetScreenHeight()-112.f;
    if(rowButton("All courses",false,true,width)) go(Screen::Catalogue);
    panel({geometry.assistantX,96,geometry.assistantWidth,
           static_cast<float>(GetScreenHeight()-156)});
    x = geometry.assistantX+16; y = 116; width = geometry.assistantWidth-32;
    heading("Study assistant");
    paragraph("Offline authored guidance",blue,14);
    const auto &l = lessons[lesson];
    const bool exercise = (stage==1 || stage==2) && !blockedFresh;
    const std::string key = l.id + ":" + std::to_string(stage) + ":" + std::to_string(question);
    const float bodyTop = y;
    const float bodyBottom = static_cast<float>(GetScreenHeight()-232);
    auto &chatScroll = assistantScroll[key];
    if(!menuOpen && GetMouseX() >= geometry.assistantX &&
       GetMouseY() >= bodyTop && GetMouseY() < bodyBottom)
      chatScroll = std::max(0.f, chatScroll - GetMouseWheelMove()*40);
    EndScissorMode();
    BeginScissorMode(static_cast<int>(x),static_cast<int>(bodyTop),static_cast<int>(width),
                     static_cast<int>(std::max(0.f,bodyBottom-bodyTop)));
    y -= chatScroll;
    assistantKey=key;
    assistantBody = true; assistantTop=bodyTop; assistantBottom=bodyBottom;
    if(blockedFresh && stage==2) {
      paragraph("This check needs saved progress before assistance is available.",muted,16);
    } else {
      if(conversations.count(key)) {
        paragraph("You",muted,13);
        paragraph(conversations[key].first,ink,16);
        paragraph("Study guide",blue,13);
        paragraph(conversations[key].second,ink,16);
      } else paragraph(exercise ? "Stuck on this question? Ask for a hint. Using hints counts as help."
                                : "Read the material in the centre. I can explain the lesson using its authored notes.",muted,16);
      auto respond = [&](const std::string &message, int level) {
        if(exercise) {
          if(!app.help(lesson,question)) {
            conversations[key] = {message,"Help could not be saved. Open Save details before revealing a hint."};
            return;
          }
          hint = level;
          conversations[key] = {message,l.questions[question].hints[level-1]};
        } else conversations[key] = {message,l.body};
        chatDraft.set("");
      };
      if(rowButton(exercise ? "1. Concept" : "Explain this lesson",false,true,width))
        respond("Explain the concept",1);
      if(exercise && rowButton("2. Guidance",false,true,width)) respond("Guide me through it",2);
      if(exercise && rowButton("3. Explanation",false,true,width)) respond("Show the explanation",3);
      assistantBody = false;
      const float chatMaximum = std::max(0.f, y + chatScroll - bodyBottom);
      chatScroll = std::min(chatScroll,chatMaximum);
      if(chatMaximum > 0) {
        float h = bodyBottom-bodyTop;
        DrawRectangleRec({x+width-3,bodyTop+chatScroll/(chatMaximum+1)*(h-24),3,24},line);
      }
      EndScissorMode();
      contentClip();
      // Keep chat entry and send controls anchored within the assistant pane.
      y = static_cast<float>(GetScreenHeight()-216);
      paragraph("Replies use lesson notes, not live AI.",muted,13);
      Rectangle field{x,y,width,44};
      int id = widgets++;
      if(!menuOpen && CheckCollisionPointRec(GetMousePosition(),field) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        focus=id; chatFocus=true; answerFocus=false;
      }
      if(tabbing && focus==id) { chatFocus=true; answerFocus=false; }
      bool active=focus==id && chatFocus && !menuOpen && windowFocused;
      DrawRectangleRounded(field,.15f,8,raised);
      DrawRectangleRoundedLinesEx(field,.15f,8,1,active?blue:line);
      if(active) {
        bool command=modifiers.command;
        bool shift=modifiers.shift;
        int ch; while((ch=GetCharPressed())!=0) if(acceptTextCharacter(ch,modifiers) && chatDraft.value().size()<160) chatDraft.insert(std::string(1,static_cast<char>(ch)));
        if(IsKeyPressed(KEY_BACKSPACE)||IsKeyPressedRepeat(KEY_BACKSPACE)) chatDraft.backspace();
        if(IsKeyPressed(KEY_LEFT)) chatDraft.move(-1,shift);
        if(IsKeyPressed(KEY_RIGHT)) chatDraft.move(1,shift);
        if(command && IsKeyPressed(KEY_A)) chatDraft.selectAll();
        if(command && IsKeyPressed(KEY_V)) {
          const char *clip=GetClipboardText();
          if(clip) chatDraft.insert(std::string(clip).substr(0,160));
        }
      }
      EndScissorMode();
      BeginScissorMode(static_cast<int>(x+8),static_cast<int>(y+8),static_cast<int>(width-16),28);
      auto value=chatDraft.value();
      if(value.size()>22) value=value.substr(value.size()-22);
      text(value.empty()?"Ask about this lesson...":value,x+8,y+14,14,value.empty()?muted:ink);
      EndScissorMode();
      contentClip();
      y += 52;
      bool sendKey=active && IsKeyPressed(KEY_ENTER);
      if(rowButton("Send",true,!chatDraft.value().empty(),width) || sendKey) {
        if(!chatDraft.value().empty()) respond(chatDraft.value(),exercise?std::min(3,hint+1):1);
        actionUsed=true;
      }
    }
    assistantBody=false;
    inCoursePanels=false;
    EndScissorMode();
    contentClip();
    x=centerX; y=centerY; width=centerWidth; inContent=wasContent;
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
      if (button(tabs[i], {x + i * (bw + 8), y, bw, 42}, false, true, true, ButtonStyle::Quiet)) {
        reviewMode = false;
        stage = i;
        question = i == 2 ? 2 : i == 1 ? practicePosition[l.id] : 0;
        clearQuestion();
        scroll = 0;
        savePosition();
        blockedFresh = stage == 2 && !app.issue(lesson, 2);
      }
    DrawRectangleRec({x + stage*(bw+8)+12,y+42,bw-24,2},blue);
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
      heading(q.prompt);
      code(questionCode, l.id + ":example", "Question code", true);
      if (q.type == QuestionType::Choice) {
        paragraph("Choose one answer, then select Check answer.",muted,16);
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
      if (!feedback.empty() && button("Try again", {x + 196, y, 144, 46}, verdict == Verdict::Incorrect)) {
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
      if (rowButton("Reveal answer", false, true, 180)) {
        if(app.help(lesson,question)) {
          hint=3;
          feedback="Answer: " + q.answer + ". " + q.explanation;
          verdict=Verdict::Invalid;
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
      y += 16;
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
  void exportReport() {
    auto result = exportProgress(store.directory() / "exports", lessons, app);
    exportFailed = !result.error.empty();
    exportNotice = exportFailed ? result.error : "Report saved: " + result.path.string();
  }
  void progress() {
    title(
        "Your progress",
        "Practice completion and fresh-check evidence describe different parts "
        "of learning. A single question is not proof of long-term mastery.");
    if (rowButton("Export progress CSV", false, true, 260)) {
      exportReport();
    }
    paragraph("Exports current question results without your typed answers. This report cannot restore a profile.", muted, 16);
    if (!exportNotice.empty()) paragraph(exportNotice, exportFailed ? red : green, 16);
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
    title("Settings", "Your reading experience, study tools and local learning data.");
    const float pageX = x, pageWidth = width;
    // Aligned setting groups share the page surface rather than nesting cards.
    auto group = [&](const std::string& label, const std::string& description) {
      x = pageX; width = pageWidth;
      y += 16;
      DrawLineEx({x,y}, {x+width,y}, 1, Color{50,61,78,255});
      y += 24;
      text(label, x, y, 20, ink);
      const float labelWidth = std::min(224.f, pageWidth * .26f);
      float labelY = y + 32;
      for (const auto& row : wrap(description, labelWidth-24, 15)) {
        text(row, x, labelY, 15, muted); labelY += 22;
      }
      x = pageX + labelWidth; width = pageWidth - labelWidth;
    };
    group("Appearance", "Make lesson text and code comfortable to read.");
    text("Text size", x, y, 17, ink);
    y += 32;
    fontField = {x,y,220,44};
    fontFieldId = widgets;
    drawingFontMenu = true;
    if (button("Font size", fontField, false, true, false)) {
      fontMenuOpen = !fontMenuOpen;
      fontOption = (app.profile.textSize-16)/2;
      fontMenuJustOpened = fontMenuOpen;
    }
    drawingFontMenu = false;
    DrawRectangleRounded(fontField, .36f, 12, fontMenuOpen || hit(fontField,GetMousePosition()) ? pale : raised);
    DrawRectangleRoundedLinesEx(fontField,.36f,12,1,fontMenuOpen ? blue : line);
    if (tabbing && focus == fontFieldId)
      DrawRectangleRoundedLinesEx({x-3,y-3,226,50},.36f,12,2,blue);
    text(std::to_string(app.profile.textSize)+" px",x+16,y+13,17,ink);
    DrawLineEx({x+190,y+19},{x+196,y+25},2,muted);
    DrawLineEx({x+196,y+25},{x+202,y+19},2,muted);
    y += 64;
    text("Reading preview", x, y, 14, muted); y += 28;
    paragraph("A variable gives a name to a value.", ink);
    const float previewSize = static_cast<float>(app.profile.textSize);
    DrawTextEx(mono, "int score = 42;", {x,y}, previewSize, 1, blue);
    y += previewSize + 24;
    group("Study assistant", "Help remains available while you study offline.");
    DrawCircle(static_cast<int>(x+4), static_cast<int>(y+10), 4, green);
    text("Offline hints available", x+18, y, 17, ink); y += 32;
    paragraph("Three authored hint levels per question. Live AI is not configured; no network requests are sent.", muted, 16);
    y += 8;
    group("Local progress", "Your attempts and resume point stay on this Mac.");
    paragraph(store.writable() ? "Local saving available" : "Temporary practice session", store.writable() ? green : red, 17);
    const auto saveMessage = app.notice.empty() ? store.status() : app.notice;
    if (!saveMessage.empty()) paragraph(saveMessage, app.unsavedChanges || !store.writable() ? red : muted, 16);
    if (store.writable() && app.unsavedChanges && rowButton("Retry save", false, true, 180)) app.save();
    if (button("View progress", {x,y,180,44})) go(Screen::Progress);
    if (button("Export progress CSV", {x+196,y,220,44})) exportReport();
    y += 60;
    if (!exportNotice.empty()) paragraph(exportNotice, exportFailed ? red : green, 15);
    paragraph("Progress folder", ink, 15);
    paragraph(store.directory().string(), muted, 15);
    if (!store.writable()) paragraph("Existing files are preserved. Changes in this session cannot be saved.", muted, 16);
    y += 8;
    group("Keyboard & access", "Use the app with a mouse or keyboard.");
    paragraph("Tab moves focus. Enter or Space activates a control. Page Up and Page Down scroll.", muted, 16);
    if (rowButton("Keyboard shortcuts", false, true, 220)) { keyboardHelp = true; scroll = 0; }
    paragraph("Single-line ASCII answers. Native VoiceOver and Unicode/IME input are not implemented.", muted, 15);
    y += 8;
    group("Reset progress", "Start again with a new local profile generation.");
    paragraph("Clears lesson progress, first-attempt evidence and mistake review. Older backup files remain on disk and are not automatically restored. This is not secure erasure.", muted, 16);
    if (!resetConfirm) {
      if (button("Reset progress...", {x, y, 220, 44}, false, store.writable(), true, ButtonStyle::Destructive))
        resetConfirm = true;
    } else {
      paragraph("Reset all lesson progress, first-attempt evidence and mistake "
                "review?",
                red);
      if (button("Confirm reset", {x, y, 180, 46}, true, true, true, ButtonStyle::Destructive)) {
        if (app.reset()) {
          drafts.clear();
          catalogueScroll.clear();
          practicePosition.clear();
          codeOffsets.clear();
          conversations.clear();
          assistantScroll.clear();
          moduleScroll.clear();
          chatDraft.set("");
        }
        resetConfirm = false;
        clearQuestion();
      }
      if (button("Cancel", {x + 196, y, 120, 46}))
        resetConfirm = false;
      y += 64;
    }
    y += 64;
    x = pageX; width = pageWidth;
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
      EndScissorMode();
      BeginScissorMode(static_cast<int>(courseLayout(GetScreenWidth()).materialX),88,
                       static_cast<int>(courseLayout(GetScreenWidth()).materialWidth),GetScreenHeight()-136);
      lessonScreen();
      EndScissorMode();
      contentClip();
      coursePanels();
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
         undoTest = false, authoredTest = false, courseTest = false, conditionsTest = false, pointerTest = false, settingsTest = false;
    int frames = 0;
    bool compact = false, largeText = false, checkContent = false;
    std::string resumeTest;
    std::string initial = "home";
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "--check-content") { checkContent = true; continue; }
      if (arg == "--settings-test") { smoke=true; settingsTest=true; continue; }
      if (arg == "--pointer-test") { smoke=true; pointerTest=true; continue; }
      if (arg == "--conditions-test") { smoke=true; conditionsTest=true; continue; }
      if (arg == "--course-test") { smoke=true; courseTest=true; continue; }
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
            << "CWaypoint [--check-content] [--data-dir PATH] [--capture-dir PATH] [--screen "
               "home|lesson|review|settings|progress|compare] "
               "[--settings-test|--pointer-test|--course-test|--smoke-test|--guided-test|--review-test|--code-test|--undo-test|--authored-test] "
               "[--resume-test "
               "seed|question|walk-seed|walk]\n";
        return 0;
      } else
        throw std::runtime_error("Unknown or incomplete option: " + arg);
    }
    if (checkContent) {
      const auto content = sampleLessons();
      auto error = validate(content);
      if (!error.empty()) { std::cerr << "Content validation failed: " << error << '\n'; return 1; }
      size_t questions = 0, hints = 0, warnings = 0;
      for (const auto& lesson : content) {
        questions += lesson.questions.size();
        for (const auto& question : lesson.questions) hints += question.hints.size();
        if (!lesson.steps.empty()) {
          auto problem = validateWalkthrough(lesson.steps);
          if (!problem.empty()) { ++warnings; std::cout << "Optional walkthrough unavailable: " << lesson.id << ": " << problem << '\n'; }
        }
      }
      const auto pairs = comparisons();
      for (const auto& pair : pairs) {
        auto problem = validateComparison(pair);
        if (!problem.empty()) { ++warnings; std::cout << "Optional comparison unavailable: " << problem << '\n'; }
      }
      std::cout << "Content valid: " << content.size() << " lessons, " << questions << " questions, " << hints << " hints, " << pairs.size() << " comparisons; " << warnings << " optional warnings.\n";
      return warnings ? 2 : 0;
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
    InitWindow(pointerTest ? 2036 : compact ? 960 : 1280, pointerTest ? 1000 : compact ? 640 : 840,
               "CWaypoint - Learn C and C++");
    if (!IsWindowReady())
      throw std::runtime_error("Unable to initialize the application window.");
    SetWindowMinSize(960, 640);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    UI ui(lessons, store, controller);
    ui.pointerHitTest = pointerTest || settingsTest;
    Image fire = LoadImageFromMemory(".png", streakFirePng, sizeof(streakFirePng));
    ImageFormat(&fire, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ui.streakFire = LoadTextureFromImage(fire);
    ImageColorTint(&fire, Color{160, 160, 160, 140});
    ImageFormat(&fire, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ui.streakInactive = LoadTextureFromImage(fire);
    UnloadImage(fire);
    SetTextureFilter(ui.streakFire, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(ui.streakInactive, TEXTURE_FILTER_BILINEAR);

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
        "Lessons", "Open C course",
        "Start practice", "Enter answer: 5",
        "Check answer",   "Next practice question",
        "1. Concept",     "Reveal answer",
        "Try again",      "Next practice question"};
    // Smoke exercises the same button handlers as mouse/keyboard; assertions
    // check persisted outcomes.
    if (smoke)
      actions = {"Lessons",
                 "Open C course",
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
                 "Export progress CSV",
                 "C / C++ comparisons",
                 "Settings",
                 "Preferences",
                 "Font size",
                 "20 px",
                 "Settings",
                 "Keyboard help",
                 "Back to preferences",
                 "Home",
                 "Lessons"};
    if (guided)
      actions = {"Open C course",
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
        actions = {"Open C course", "Start practice", "Enter answer: 5",
                   "Check answer", "Next practice question"};
      else if (resumeTest == "walk-seed")
        actions = {"Continue lesson", "Read", "Next step", "Next step"};
      else
        actions = {"Continue lesson"};
    }
    if (codeTest)
      actions = {
          "Open C course", "Scroll right", "Scroll right", "Read",
          "Start practice",           "Scroll left",  "Home"};
    if (undoTest)
      actions = {"Open C course",
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
      actions = {"Open C course", "Start practice",
                 "Enter answer: wrong", "Check answer", "1. Concept",
                 "Try again", "Enter answer: 5", "Check answer",
                 "Next practice question", "Enter answer: int", "Check answer",
                 "Try the fresh check", "Enter answer: 4", "Check answer",
                 "View summary", "Progress"};
    if(courseTest)
      actions={"Lessons","Open C++ course","Start practice","Enter answer: Hi Ada",
               "Check answer","All courses","Open C course","Module: Pointers & addresses",
               "Start practice","&value","Check answer","All courses","Open C course",
               "Start practice","Chat: Help me understand assignment","Send",
               "Settings","Preferences","Reset progress...","Confirm reset",
               "Lessons","Open C course","Start practice"};
    if(settingsTest)
      actions={"Settings","Preferences","Font size","24 px","Font size","16 px",
               "Font size","18 px","Font size","22 px","Font size","20 px",
               "Keyboard shortcuts","Back to preferences"};
    if(conditionsTest)
      actions={"Lessons","Open C course","Module: Conditions: if and else", "Start practice",
               "Pass","Check answer","Next practice question","Enter answer: Retry","Check answer",
               "Try the fresh check","Enter answer: closed","Check answer","View summary"};
    if(pointerTest)
      actions={"Lessons","Open C course","Next step","Next step","Previous","Start practice","Enter answer: 5","Check answer",
               "Next practice question","Enter answer: int","Check answer","Summary",
               "All courses","Open C++ course","Practice","Enter answer: Hi Ada","Check answer",
               "Settings","Preferences","Home"};
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
    bool undoBarrierOk = true, courseAssistanceOk = false;
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
      if (smoke && !guided && !reviewTest && !codeTest && !undoTest && !authoredTest && !courseTest && !conditionsTest && !pointerTest && !settingsTest &&
          resumeTest.empty() && action == 1 && !ui.automaticAction.empty()) {
        ui.scroll = std::min(80.f, ui.maxScroll);
        expectedCatalogueScroll = ui.scroll;
      }
      if (smoke && !guided && !reviewTest && !codeTest && !undoTest && !authoredTest && !courseTest && !conditionsTest && !pointerTest && !settingsTest &&
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
      const bool scriptedChat = ui.automaticAction.rfind("Chat: ",0)==0;
      if(scriptedChat) {
        if(ui.screen!=Screen::Lesson) throw std::runtime_error("Chat scenario needs a course");
        ui.chatDraft.set(ui.automaticAction.substr(6));
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
      if (settingsTest && ui.automaticAction == "Keyboard shortcuts") ui.scroll = ui.maxScroll;
      if (settingsTest && ui.automaticAction == "Back to preferences") ui.scroll = 0;
      if (courseTest && !pendingCapture.empty() && (action == 19 || action == 20))
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
      if (!capture.empty() && (captureTarget.texture.width != GetScreenWidth() ||
                                captureTarget.texture.height != GetScreenHeight())) {
        UnloadRenderTexture(captureTarget);
        captureTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
      }
      if (!capture.empty())
        BeginTextureMode(captureTarget);
      else
        BeginDrawing();
      ClearBackground(paper);
      ui.draw();
      if (scriptedInput || scriptedChat)
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
          if (settingsTest && action >= 4 && action <= 12 && action % 2 == 0) {
            const int expected[] = {24,16,18,22,20};
            if (controller.profile.textSize != expected[(action-4)/2] ||
                store.load().textSize != expected[(action-4)/2] || ui.fontMenuOpen)
              throw std::runtime_error("Font selection did not close and persist the chosen size.");
          }
          if (pointerTest && action >= 3 && action <= 5) {
            const int expectedStep = action == 4 ? 2 : 1;
            if (ui.walk != expectedStep || controller.profile.walkthrough != expectedStep)
              throw std::runtime_error("Walkthrough button did not update the saved step.");
          }
          if(courseTest && action==16)
            courseAssistanceOk=controller.solved(1,0) && controller.solved(2,0) &&
              controller.profile.evidence.at("c-var-p1").helped &&
              controller.profile.evidence.at("c-var-p1").attempts==0 &&
              ui.conversations.at("c-variables:1:0").first=="Help me understand assignment" &&
              ui.conversations.at("c-variables:1:0").second==lessons[0].questions[0].hints[0];
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
        if (!guided && !reviewTest && !codeTest && !undoTest && !authoredTest && !courseTest && !conditionsTest && !pointerTest && !settingsTest &&
            resumeTest.empty())
          testOk = controller.completed(0) == 1 &&
                   controller.profile.evidence.at("c-var-p2").helped &&
                   controller.profile.evidence.at("c-var-fresh").helped &&
                   controller.profile.textSize == 20 &&
                   ui.lastWidgets == ui.widgets &&
                   ui.drafts.at("c-var-p2").value() == "int" &&
                   ui.practicePosition.at("c-variables") == 1 &&
                   std::abs(ui.scroll - expectedCatalogueScroll) < 1;
        if(pointerTest)
          testOk=GetScreenWidth()>=1600 && controller.completed(0)==2 &&
            controller.solved(1,0) && ui.screen==Screen::Home && !ui.menuOpen;
        if (pointerTest && !testOk)
          std::cerr << "Pointer result: width=" << GetScreenWidth()
                    << " C practice=" << controller.completed(0)
                    << " C++ solved=" << controller.solved(1,0)
                    << " screen=" << static_cast<int>(ui.screen)
                    << " menu=" << ui.menuOpen << '\n';
        if(settingsTest)
          testOk=controller.profile.textSize==20 && store.load().textSize==20 && ui.screen==Screen::Settings;
        if(conditionsTest)
          testOk=controller.completed(3)==2 && controller.solved(3,2) &&
            controller.status(3,2)=="Solved independently on first attempt" &&
            controller.profile.studyDays.count(localStudyDay())==1 && ui.stage==3;
        if(courseTest)
          testOk=courseAssistanceOk && ui.conversations.empty() &&
            ui.chatDraft.value().empty() && !controller.profile.evidence.count("c-var-p1") &&
            ui.screen==Screen::Lesson && ui.lesson==0 && ui.stage==1;
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
                   controller.nextLesson("C") == 3 &&
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
    UnloadTexture(ui.streakFire);
    UnloadTexture(ui.streakInactive);
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
