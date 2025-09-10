// src/gui_main.cpp  (SFML 3.x)
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <cmath>

#include "chess/types.hpp"
#include "chess/position.hpp"
#include "chess/move.hpp"
#include "chess/movegen.hpp"
#include "chess/make_undo.hpp"
#include "chess/status.hpp"
#include "chess/fen.hpp"

static inline int file_of(int sq) { return sq % 8; }
static inline int rank_of(int sq) { return sq / 8; }
static inline int sq_from_fr(int f, int r) { return r * 8 + f; }
static inline bool in_bounds(int f, int r) { return (f >= 0 && f < 8 && r >= 0 && r < 8); }

static sf::String piece_glyph(Piece p) {
    switch (p) {
        case WK: return sf::String(U"\u2654");
        case WQ: return sf::String(U"\u2655");
        case WR: return sf::String(U"\u2656");
        case WB: return sf::String(U"\u2657");
        case WN: return sf::String(U"\u2658");
        case WP: return sf::String(U"\u2659");
        case BK: return sf::String(U"\u265A");
        case BQ: return sf::String(U"\u265B");
        case BR: return sf::String(U"\u265C");
        case BB: return sf::String(U"\u265D");
        case BN: return sf::String(U"\u265E");
        case BP: return sf::String(U"\u265F");
        default: return sf::String();
    }
}

struct Colors {
    sf::Color light = sf::Color(240, 217, 181);
    sf::Color dark  = sf::Color(181, 136, 99);
    sf::Color select = sf::Color(102, 153, 255, 120);
    sf::Color target = sf::Color(0, 0, 0, 70);
    sf::Color legalCapture = sf::Color(255, 0, 0, 90);
    sf::Color check = sf::Color(255, 40, 40, 120);
};

struct UiState {
    std::optional<int> selected;
    std::vector<Move> legal_for_sel;
    std::optional<Move> lastMove;
    bool gameOver = false;
    GameStatus status{};
};

static std::vector<Move> filter_from(const std::vector<Move>& all, int from) {
    std::vector<Move> out;
    out.reserve(all.size());
    for (const auto& m : all) if (m.from == from) out.push_back(m);
    return out;
}

static bool try_pick_promotion(const std::vector<Move>& choices, int to, Move& out) {
    for (const auto& m : choices)
        if ((m.flags & PROMOTION) && m.to == to && m.promo == PROMO_Q) { out = m; return true; }
    for (const auto& m : choices)
        if ((m.flags & PROMOTION) && m.to == to) { out = m; return true; }
    return false;
}

static int square_from_mouse(int boardSize, int margin, sf::Vector2i mouse) {
    int left = margin, top = margin, right = margin + boardSize, bottom = margin + boardSize;
    if (mouse.x < left || mouse.x >= right || mouse.y < top || mouse.y >= bottom) return -1;
    int sqSize = boardSize / 8;
    int f = (mouse.x - left) / sqSize;
    int r = 7 - ((mouse.y - top) / sqSize);
    return sq_from_fr(f, r);
}

int main() {
    constexpr unsigned BOARD_PIXELS = 800u;
    constexpr int      MARGIN       = 20;
    constexpr int      SIDE_PANEL   = 280;
    const unsigned     WIDTH  = static_cast<unsigned>(MARGIN + BOARD_PIXELS + MARGIN + SIDE_PANEL + MARGIN);
    const unsigned     HEIGHT = static_cast<unsigned>(MARGIN + BOARD_PIXELS + MARGIN);

    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}, 32u), "Chess GUI (SFML)");
    window.setFramerateLimit(120);

    sf::Font font;
    if (!font.openFromFile("assets/DejaVuSans.ttf")) {
    }

    Colors colors;
    Position pos;
    pos.start_position();
    UiState ui;

    auto recompute_status = [&]() {
        ui.status = assessStatus(pos);
        ui.gameOver = (ui.status.phase == Phase::GameOver);
    };
    recompute_status();

    auto draw_board = [&](sf::RenderTarget& rt) {
        int sqSize = BOARD_PIXELS / 8;
        sf::RectangleShape rect({static_cast<float>(sqSize), static_cast<float>(sqSize)});

        for (int r = 7; r >= 0; --r) {
            for (int f = 0; f < 8; ++f) {
                bool light = ((r + f) % 2 == 0);
                rect.setFillColor(light ? colors.light : colors.dark);
                rect.setPosition({static_cast<float>(MARGIN + f * sqSize),
                                  static_cast<float>(MARGIN + (7 - r) * sqSize)});
                rt.draw(rect);
            }
        }
        if (ui.status.in_check) {
            int ks = (pos.side_to_move == WHITE) ? peek_lsb(pos.K) : peek_lsb(pos.k);
            if (ks >= 0) {
                sf::RectangleShape hl({static_cast<float>(sqSize), static_cast<float>(sqSize)});
                hl.setFillColor(colors.check);
                hl.setPosition({static_cast<float>(MARGIN + file_of(ks) * sqSize),
                                static_cast<float>(MARGIN + (7 - rank_of(ks)) * sqSize)});
                rt.draw(hl);
            }
        }
        if (ui.selected.has_value()) {
            int sel = *ui.selected;
            sf::RectangleShape selr({static_cast<float>(sqSize), static_cast<float>(sqSize)});
            selr.setFillColor(colors.select);
            selr.setPosition({static_cast<float>(MARGIN + file_of(sel) * sqSize),
                              static_cast<float>(MARGIN + (7 - rank_of(sel)) * sqSize)});
            rt.draw(selr);

            for (const auto& m : ui.legal_for_sel) {
                int t = m.to;
                sf::CircleShape dot(static_cast<float>(sqSize) * 0.18f);
                dot.setOrigin({dot.getRadius(), dot.getRadius()});
                dot.setPosition({static_cast<float>(MARGIN + file_of(t) * sqSize + sqSize / 2),
                                 static_cast<float>(MARGIN + (7 - rank_of(t)) * sqSize + sqSize / 2)});
                bool isCap = (m.flags & CAPTURE) || (m.flags & EN_PASSANT);
                dot.setFillColor(isCap ? colors.legalCapture : colors.target);
                rt.draw(dot);
            }
        }
    };

    auto draw_pieces = [&](sf::RenderTarget& rt) {
        int sqSize = BOARD_PIXELS / 8;
        unsigned glyphSize = static_cast<unsigned>(std::round(sqSize * 0.8f));
        sf::Text glyph(font, sf::String(), glyphSize);  
        glyph.setFillColor(sf::Color::Black);

        for (int sq = 0; sq < 64; ++sq) {
            Piece pc = getPiece(pos, sq);
            if (pc == EMPTY) continue;

            glyph.setString(piece_glyph(pc));

            float cx = static_cast<float>(MARGIN + file_of(sq) * sqSize + sqSize * 0.5f);
            float cy = static_cast<float>(MARGIN + (7 - rank_of(sq)) * sqSize + sqSize * 0.5f);

            auto bounds = glyph.getLocalBounds(); 
            glyph.setOrigin({bounds.position.x + bounds.size.x * 0.5f,
                             bounds.position.y + bounds.size.y * 0.85f});
            glyph.setPosition({cx, cy});
            rt.draw(glyph);
        }
    };

    auto draw_sidebar = [&](sf::RenderTarget& rt) {
        int left = MARGIN + static_cast<int>(BOARD_PIXELS) + MARGIN;
        sf::RectangleShape bg({static_cast<float>(SIDE_PANEL), static_cast<float>(BOARD_PIXELS)});
        bg.setPosition({static_cast<float>(left), static_cast<float>(MARGIN)});
        bg.setFillColor(sf::Color(240, 240, 245));
        rt.draw(bg);

        sf::Text t(font, sf::String(), 20);    
        t.setFillColor(sf::Color::Black);

        t.setString(sf::String(std::string("Turn: ") + (pos.side_to_move == WHITE ? "White" : "Black")));
        t.setPosition({static_cast<float>(left + 16), static_cast<float>(MARGIN + 16)});
        rt.draw(t);

        t.setPosition({static_cast<float>(left + 16), static_cast<float>(MARGIN + 48)});
        if (ui.gameOver) {
            std::string msg = "Game Over: ";
            switch (ui.status.outcome) {
                case Outcome::Whitewins: msg += "White wins (checkmate)"; break;
                case Outcome::Blackwins: msg += "Black wins (checkmate)"; break;
                case Outcome::Draw:
                    msg += "Draw";
                    if (ui.status.draw_reason == DrawReason::Stalemate) msg += " (stalemate)";
                    else if (ui.status.draw_reason == DrawReason::FiftyMove) msg += " (50-move)";
                    else if (ui.status.draw_reason == DrawReason::Threefold) msg += " (threefold)";
                    else if (ui.status.draw_reason == DrawReason::InsufficientMaterial) msg += " (insufficient)";
                    else if (ui.status.draw_reason == DrawReason::Agreement) msg += " (agreement)";
                    break;
                default: msg += "Unknown";
            }
            t.setString(sf::String(msg));
        } else if (ui.status.in_check) {
            t.setString(sf::String("Check!"));
        } else {
            t.setString(sf::String("Playing..."));
        }
        rt.draw(t);

        sf::Text help(font, sf::String(), 16);
        help.setFillColor(sf::Color::Black);
        help.setPosition({static_cast<float>(left + 16), static_cast<float>(MARGIN + 100)});
        help.setString(sf::String("Controls:\n"
                                  "- Left click: select/move\n"
                                  "- Right click: cancel selection\n"
                                  "- U: undo\n"
                                  "- R: reset to start"));
        rt.draw(help);
    };

    auto handle_click = [&](int sq) {
        if (sq < 0 || sq > 63 || ui.gameOver) return;

        if (!ui.selected.has_value()) {
            Piece pc = getPiece(pos, sq);
            if (pc == EMPTY) return;
            bool isWhite = (pc >= WP && pc <= WK);
            if ((pos.side_to_move == WHITE && !isWhite) || (pos.side_to_move == BLACK && isWhite))
                return;

            std::vector<Move> all;
            generateLegalAllMoves(pos, all);
            auto fr = filter_from(all, sq);
            if (fr.empty()) return;

            ui.selected = sq;
            ui.legal_for_sel = std::move(fr);
            return;
        }

        std::vector<Move> candidates;
        for (const auto& m : ui.legal_for_sel) if (m.to == sq) candidates.push_back(m);
        if (candidates.empty()) {
            Piece pc = getPiece(pos, sq);
            if (pc != EMPTY) {
                bool isWhite = (pc >= WP && pc <= WK);
                if ((pos.side_to_move == WHITE && isWhite) || (pos.side_to_move == BLACK && !isWhite)) {
                    std::vector<Move> all;
                    generateLegalAllMoves(pos, all);
                    ui.selected = sq;
                    ui.legal_for_sel = filter_from(all, sq);
                    return;
                }
            }
            ui.selected.reset();
            ui.legal_for_sel.clear();
            return;
        }

        Move chosen = candidates.front();
        if (chosen.flags & PROMOTION) {
            if (!try_pick_promotion(candidates, sq, chosen)) {
                chosen = candidates.front();
            }
        }

        makeMove(pos, chosen);
        ui.lastMove = chosen;
        ui.selected.reset();
        ui.legal_for_sel.clear();
        recompute_status();
    };

    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::U) {
                    if (!ui.gameOver) {
                        extern std::stack<MoveHistory> history;
                        if (!history.empty()) {
                            UndoMove(pos);
                            ui.lastMove.reset();
                            recompute_status();
                        }
                    }
                }
                if (key->code == sf::Keyboard::Key::R) {
                    pos.start_position();
                    ui = UiState{};
                    recompute_status();
                }
            }
            if (auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    int sq = square_from_mouse(static_cast<int>(BOARD_PIXELS), MARGIN, mb->position);
                    handle_click(sq);
                } else if (mb->button == sf::Mouse::Button::Right) {
                    ui.selected.reset();
                    ui.legal_for_sel.clear();
                }
            }
        }

        window.clear(sf::Color(30, 30, 35));
        draw_board(window);
        draw_pieces(window);
        draw_sidebar(window);
        window.display();
    }
    return 0;
}
