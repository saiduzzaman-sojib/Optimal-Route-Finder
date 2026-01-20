#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <limits>
#include <optional>

using namespace std;

struct Point {
    float x, y;
};

int getDistance(Point p1, Point p2) {
    return static_cast<int>(sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
}

const int INF = 1e9;
int n;
vector<vector<int>> distMatrix;
vector<vector<int>> memo;
vector<vector<int>> parent;

int tsp(int mask, int pos) {
    if (mask == (1 << n) - 1) return distMatrix[pos][0];
    if (memo[mask][pos] != -1) return memo[mask][pos];

    int ans = INF;
    for (int city = 0; city < n; city++) {
        if ((mask & (1 << city)) == 0) {
            int newAns = distMatrix[pos][city] + tsp(mask | (1 << city), city);
            if (newAns < ans) {
                ans = newAns;
                parent[mask][pos] = city;
            }
        }
    }
    return memo[mask][pos] = ans;
}

void solveTSP(const vector<Point>& cities, vector<int>& resultPath) {
    n = cities.size();
    if (n == 0) return;

    distMatrix.assign(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distMatrix[i][j] = getDistance(cities[i], cities[j]);
        }
    }

    memo.assign(1 << n, vector<int>(n, -1));
    parent.assign(1 << n, vector<int>(n, -1));

    tsp(1, 0);

    resultPath.clear();
    int mask = 1, pos = 0;
    resultPath.push_back(0);
    while (true) {
        int nextCity = parent[mask][pos];
        if (nextCity == -1) break;
        resultPath.push_back(nextCity);
        mask |= (1 << nextCity);
        pos = nextCity;
    }
    resultPath.push_back(0);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 800}), "Optimal Route Finder");
    window.setFramerateLimit(60);

    vector<Point> cities;
    vector<int> optimalPath;
    bool calculated = false;

    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        cerr << "Warning: System font not found." << endl;
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                     if (!calculated) {
                        cities.push_back({static_cast<float>(mouseBtn->position.x), static_cast<float>(mouseBtn->position.y)});
                    }
                }
            }
            else if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->code == sf::Keyboard::Key::Enter) {
                    if (!cities.empty()) {
                        solveTSP(cities, optimalPath);
                        calculated = true;
                    }
                }
                else if (keyPress->code == sf::Keyboard::Key::R) {
                    cities.clear();
                    optimalPath.clear();
                    calculated = false;
                }
            }
        }

        window.clear(sf::Color(30, 30, 40)); 

        if (calculated && !optimalPath.empty()) {
            for (size_t i = 0; i < optimalPath.size() - 1; i++) {
                int u = optimalPath[i];
                int v = optimalPath[i+1];
                
                sf::Vertex line[] = {
                    sf::Vertex{ sf::Vector2f{cities[u].x, cities[u].y}, sf::Color::Cyan },
                    sf::Vertex{ sf::Vector2f{cities[v].x, cities[v].y}, sf::Color::Cyan }
                };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }

        for (size_t i = 0; i < cities.size(); i++) {
            sf::CircleShape node(10);
            node.setOrigin({10, 10}); 
            node.setPosition({cities[i].x, cities[i].y}); 
            
            if (i == 0) node.setFillColor(sf::Color(255, 80, 80)); 
            else node.setFillColor(sf::Color::White);
            
            window.draw(node);
        }

        sf::Text text(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color(200, 200, 200));
        text.setString("[Click] Add Location  |  [ENTER] Optimize Route  |  [R] Reset");
        text.setPosition({20, 20});
        window.draw(text);

        window.display();
    }

    return 0;
}
