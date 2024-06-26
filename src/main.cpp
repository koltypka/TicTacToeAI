﻿#include <vector>
#include <iostream>
#include <numeric>
#include <SFML/Graphics.hpp>

#define BLANK 0
#define CROSS 1
#define ZERO -1

#define CROSS_WON 10
#define ZERO_WON 11
#define TIE 12
#define GAME_NOT_OVER 13

using namespace sf;
using namespace std;

int game_situation(const vector<int>& board)
{
    int cortage = 0;
    int column = 0;
    int diagonal = 0;
    int negative_diagonal = 0;
    for (int i = 0; i < 8; i += 3)
    {
        cortage = board[i] + board[i + 1] + board[i + 2];
        column = board[i] + board[i + 3] + board[i + 6];
       
        if (cortage == -3) return ZERO_WON;
        if (cortage == 3) return CROSS_WON;

        if (column == -3) return ZERO_WON;
        if (column == 3) return CROSS_WON;
    }
    diagonal = board[0] + board[4] + board[8];
    negative_diagonal = board[2] + board[4] + board[6];

    if (diagonal == -3) return ZERO_WON;
    if (diagonal == 3) return CROSS_WON;

    if (negative_diagonal == -3) return ZERO_WON;
    if (negative_diagonal == 3) return CROSS_WON;
    
    for (auto i : board)
    {
        if (i == BLANK) return GAME_NOT_OVER;
    }
    return TIE;
}

bool cmp_score(int cur_score, int& score, int alpha, int beta, bool is_zero)
{
    if (is_zero)
    {
        score = min(score, cur_score);
        return (min(beta, cur_score) <= alpha);
    }

    score = max(score, cur_score);
    return (beta <= max(alpha, cur_score));
}

int minimax(vector<int>& board, int alpha, int beta, bool is_zero) 
{
    int gs = game_situation(board);
    if (gs == CROSS_WON) return 1000;
    if (gs == ZERO_WON) return -1000;
    if (gs == TIE) return 0;

    int score = -1e9;
    int tmp_value = CROSS;

    if (is_zero)
    {
        score = 1e9;
        tmp_value = ZERO;
    }

    for (int i = 0; i < 9; i++)
    {
        if (board[i] != BLANK) continue;
        vector<int> tmp = board;
        tmp[i] = tmp_value;
        int cur_score = minimax(tmp, alpha, beta, !is_zero);
        if (cmp_score(cur_score, score, alpha, beta, is_zero)) {
            break;
        }
    }

    return score;
}

int main()
{
    RenderWindow window(sf::VideoMode(850, 850), "Tic Tac Toe");
    
    Texture field_tex;
    field_tex.loadFromFile("../image/field.png");
    Sprite field_spr;
    field_spr.setTexture(field_tex);
    
    Texture cross_tex;
    cross_tex.loadFromFile("../image/cross.png");

    Texture zero_tex;
    zero_tex.loadFromFile("../image/zero.png");

    vector<int> board(9, BLANK);
    int cnt = 0;
    bool game_over = false;

    while (window.isOpen())
    {
        int gs = game_situation(board);
        if (gs != GAME_NOT_OVER && !game_over)
        {
            switch (gs)
            {
            case CROSS_WON: cout << "CROSS WON" << endl; break;
            case ZERO_WON: cout << "ZERO WON" << endl; break;
            case TIE: cout << "TIE" << endl; break;
            }
            game_over = true;
        }
        
        if (cnt % 2 == 1)
        {
            int best_move, min_eval = 1e9;
            for (int i = 0; i < 9; i++)
            {
                if (board[i] == BLANK)
                {
                    vector<int> tmp = board;
                    tmp[i] = ZERO;
                    int cur = minimax(tmp, -1e9, 1e9, false);
                    if (cur < min_eval)
                    {
                        min_eval = cur;
                        best_move = i;
                    }
                }
            }
            board[best_move] = ZERO;
            cnt++;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed: window.close(); break;
            case Event::KeyPressed:
                if (event.key.code == Keyboard::R)
                {
                    board = vector<int>(9, BLANK);
                    cnt = 0; game_over = false;
                    system("cls");
                }
                break;
            case Event::MouseButtonPressed:
                if (cnt % 2 == 0)
                {
                    if (game_over) continue;
                    Vector2f ev = Vector2f(event.mouseButton.x < 825 ? event.mouseButton.x : 825,
                        event.mouseButton.y < 825 ? event.mouseButton.y : 825);
                    int index = int((ev.y) / 275) * 3 + int((ev.x) / 275);
                    if (board[index] == BLANK) { board[index] = CROSS; cnt++; }
                }
                break;
            }
        }

        vector<Sprite> sprites(9);
        for (int i = 0; i < 9; i++)
        {
            if (board[i] == CROSS) sprites[i].setTexture(cross_tex);
            if (board[i] == ZERO)  sprites[i].setTexture(zero_tex);
            sprites[i].setPosition(Vector2f(25 + 275 * (i % 3), 25 + 275 * (i / 3)));
        }

        window.clear();
        window.draw(field_spr);
        for (auto i : sprites) window.draw(i);
        window.display();
    }

    return 0;
}