// vim:filetype=cpp:textwidth=120:shiftwidth=2:softtabstop=2:expandtab
// Copyright 2014 Christoph Schwering
//
// Command line application that plays minesweeper.

#include <cassert>

#include <iostream>
#include <string>

#include "agent.h"
#include "game.h"
#include "kb.h"
#include "printer.h"
#include "timer.h"

struct Logger {
  void explored(Point p, int k) const {
    std::cout << "Exploring";
    Rest(p, k);
  }

  void flagged(Point p, int k) const {
    std::cout << "Flagging";
    Rest(p, k);
  }

 private:
  void Rest(Point p, int k) const {
    std::cout << " " << Point(p.x+1, p.y+1);
    if (first_ && k == -1) {
      std::cout << ", chosen at random.";
    } else if (k == -1) {
      using limbo::format::operator<<;
      std::cout << ", which is just a guess.";
    } else {
      std::cout << ", found at split level " << k << ".";
    }
    first_ = false;
  }

  mutable bool first_ = false;
};

inline bool Play(size_t width, size_t height, size_t n_mines, size_t seed, size_t max_k,
                 const Colors& colors, std::ostream* os) {
  Timer overall_timer;
  Game g(width, height, n_mines, seed);
  KnowledgeBase kb(&g, max_k);
  Agent<Logger> agent(&g, &kb);
  SimplePrinter printer(&colors, os);
  OmniscientPrinter final_printer(&colors, os);
  std::vector<int> split_counts;
  split_counts.resize(max_k + 2);  // last one is for guesses
  do {
    Timer turn_timer;
    turn_timer.start();
    overall_timer.start();
    const int k = agent.Explore();
    overall_timer.stop();
    turn_timer.stop();
    if (k >= 0) {
      ++split_counts[k];
    }
    *os << std::endl;
    printer.Print(g);
    *os << std::endl;
    *os << "Last move took " << std::fixed << turn_timer.duration() << ", queries took " << std::fixed << kb.timer().duration() << " / " << std::setw(4) << kb.timer().rounds() << " = " << std::fixed << kb.timer().avg_duration() << std::endl;
    kb.ResetTimer();
  } while (!g.hit_mine() && !g.all_explored());
  *os << "Final board:" << std::endl;
  *os << std::endl;
  final_printer.Print(g);
  *os << std::endl;
  const bool win = !g.hit_mine();
  if (win) {
    *os << colors.green() << "You win :-)";
  } else {
    *os << colors.red() << "You loose :-(";
  }
  *os << "  [width: " << g.width() << "; height: " << g.height() << "; height: " << g.n_mines() << "; seed: " << g.seed() << "; max-k: " << max_k << "; ";
  for (size_t k = 0; k < split_counts.size(); ++k) {
    const int n = split_counts[k];
    if (n > 0) {
      if (k == max_k + 1) {
        std::cout << "guesses: " << n << "; ";
      } else {
        std::cout << "level " << k << ": " << n << "; ";
      }
    }
  }
  *os << "runtime: " << overall_timer.duration() << " seconds]" << colors.reset() << std::endl;
  return win;
}

int main(int argc, char *argv[]) {
  size_t width = 8;
  size_t height = 8;
  size_t n_mines = 10;
  size_t seed = 0;
  size_t max_k = 2;
  if (argc >= 2) {
    width = atoi(argv[1]);
  }
  if (argc >= 3) {
    height = atoi(argv[2]);
  }
  n_mines = (width + 1) * (height + 1) / 10;
  if (argc >= 4) {
    n_mines = atoi(argv[3]);
  }
  if (argc >= 5) {
    seed = atoi(argv[4]);
  }
  if (argc >= 6) {
    max_k = atoi(argv[5]);
  }
  Play(width, height, n_mines, seed, max_k, TerminalColors(), &std::cout);
  return 0;
}

