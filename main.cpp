#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace std;

struct Node{
    bool obstacle = false;
    bool visited  = false;
    Node* parent  = nullptr;
    float gCost   = INT_MAX;
    float hCost;
    float fCost;
    int x;
    int y;
};
//global variables
const int WIDTH = 1600, HEIGHT = 800, NODE_SIZE = 50;
const int GRID_WIDTH = WIDTH/NODE_SIZE, GRID_HEIGHT = HEIGHT/NODE_SIZE;
Node *startNode, *endNode;
Node nodes[GRID_WIDTH][GRID_HEIGHT];
sf::RectangleShape boxes[GRID_WIDTH][GRID_HEIGHT];

float distance(int x, int y){
    return sqrt(pow(endNode->x - x, 2) + pow(endNode->y - y, 2));
}

void findPath();
void colorPath();
void resetGrid();
void changeStart(int, int);
void changeEnd(int, int);
void calculateHeuristic();

int main()
{
    //set up default endNode
    endNode = &nodes[GRID_WIDTH-1][GRID_HEIGHT-1];
    endNode->visited = true;
    //set up default startNode
    startNode = &nodes[0][0];
    startNode->gCost   = 0;
    startNode->visited = true;
    //calculate all the hCost and set locations
    for(int x = 0; x < GRID_WIDTH; x++){
        for(int y = 0; y < GRID_HEIGHT; y++){
            nodes[x][y].hCost = distance(x, y);
            nodes[x][y].x     = x;
            nodes[x][y].y     = y;
        }
    }
    //Graphics and window
    for(int x = 0; x < GRID_WIDTH; x++){
        for(int y = 0; y < GRID_HEIGHT; y++){
            boxes[x][y].setPosition(x * NODE_SIZE, y * NODE_SIZE);
            boxes[x][y].setSize(sf::Vector2f(NODE_SIZE, NODE_SIZE));
            boxes[x][y].setOutlineColor(sf::Color(179, 179, 204));
            boxes[x][y].setOutlineThickness(-1);
            boxes[x][y].setFillColor(sf::Color::White);
        }
    }
    boxes[startNode->x][startNode->y].setFillColor(sf::Color::Green);
    boxes[endNode  ->x][endNode  ->y].setFillColor(sf::Color::Red);


    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "A* Star Visualization");
    bool mouseDown = false, leftMouse = false;
    //main loop
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            switch(event.type){
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:{
                if(event.key.code == sf::Keyboard::Space){
                    resetGrid();
                    findPath();
                    colorPath();
                    break;
                }
                else if(event.key.code == sf::Keyboard::R){
                    resetGrid();
                    break;
                }
                int x = sf::Mouse::getPosition(window).x / NODE_SIZE;
                int y = sf::Mouse::getPosition(window).y / NODE_SIZE;
                if(x >= 0 && x < GRID_WIDTH){
                    if(y >= 0 && y < GRID_HEIGHT){
                        if(event.key.code == sf::Keyboard::S){
                            resetGrid();
                            changeStart(x, y);
                        }
                        else if(event.key.code == sf::Keyboard::E){
                            resetGrid();
                            changeEnd(x, y);
                            calculateHeuristic();
                        }
                    }
                }
                break;
            }
            case sf::Event::MouseButtonPressed:{
                mouseDown = true;
                int x = event.mouseButton.x / NODE_SIZE;
                int y = event.mouseButton.y / NODE_SIZE;
                if((x != startNode->x || y != startNode->y) &&
                   (x != endNode  ->x || y != endNode  ->y)){
                    if(x >= 0 && x < GRID_WIDTH){
                        if(y >= 0 && y < GRID_HEIGHT){
                            if(event.mouseButton.button == sf::Mouse::Left){
                                nodes[x][y].obstacle = true;
                                boxes[x][y].setFillColor(sf::Color::Black);
                                leftMouse = true;
                            }
                            else{
                                nodes[x][y].obstacle = false;
                                boxes[x][y].setFillColor(sf::Color::White);
                                leftMouse = false;
                            }
                        }
                    }
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
                mouseDown = false;
                leftMouse = false;
                break;
            case sf::Event::MouseMoved:{
                if(mouseDown){
                    int x = event.mouseMove.x / NODE_SIZE;
                    int y = event.mouseMove.y / NODE_SIZE;
                    if((x != startNode->x || y != startNode->y) &&
                       (x != endNode->x || y != endNode->y)){
                        if(x >= 0 && x < GRID_WIDTH){
                            if(y >= 0 && y < GRID_HEIGHT){
                                if(leftMouse){
                                    nodes[x][y].obstacle = true;
                                    boxes[x][y].setFillColor(sf::Color::Black);
                                }
                                else{
                                    nodes[x][y].obstacle = false;
                                    boxes[x][y].setFillColor(sf::Color::White);
                                }
                            }
                        }
                    }
                }
            }
            }
        }
        //draw nodes/boxes to window
        window.clear();
        for(int x = 0; x < GRID_WIDTH; x++){
            for(int y = 0; y < GRID_HEIGHT; y++){
                window.draw(boxes[x][y]);
            }
        }
        window.display();
    }
    return EXIT_SUCCESS;
}
//A* algorithm, uses global variable nodes
void findPath(){
    vector<Node*> List;
    List.push_back(startNode);
    while(!List.empty()){
        //ignore endNode if it comes up
        if(List[0] == endNode){
            List.erase(List.begin());
            continue;
        }
        Node* parent = List[0];
        //go through neighbors
        int x = List[0]->x;
        int y = List[0]->y;
        if((x - 1) >= 0 && !nodes[x-1][y].obstacle){
            if(List[0]->gCost + 1 < nodes[x-1][y].gCost){
                nodes[x-1][y].gCost  = List[0]->gCost + 1;
                nodes[x-1][y].parent = parent;
                nodes[x-1][y].fCost  = nodes[x-1][y].gCost + nodes[x-1][y].hCost;
                if(!nodes[x-1][y].visited)
                    List.push_back(&nodes[x-1][y]);
                nodes[x-1][y].visited = true;
            }
        }
        if((x + 1) < GRID_WIDTH && !nodes[x+1][y].obstacle){
            if(List[0]->gCost + 1 < nodes[x+1][y].gCost){
                nodes[x+1][y].gCost  = List[0]->gCost + 1;
                nodes[x+1][y].parent = parent;
                nodes[x+1][y].fCost  = nodes[x+1][y].gCost + nodes[x+1][y].hCost;
                if(!nodes[x+1][y].visited)
                    List.push_back(&nodes[x+1][y]);
                nodes[x+1][y].visited = true;
            }
        }
        if((y - 1) >= 0 && !nodes[x][y-1].obstacle){
            if(List[0]->gCost + 1 < nodes[x][y-1].gCost){
                nodes[x][y-1].gCost  = List[0]->gCost + 1;
                nodes[x][y-1].parent = parent;
                nodes[x][y-1].fCost  = nodes[x][y-1].gCost + nodes[x][y-1].hCost;
                if(!nodes[x][y-1].visited)
                    List.push_back(&nodes[x][y-1]);
                nodes[x][y-1].visited = true;
            }
        }
        if((y + 1) < GRID_HEIGHT && !nodes[x][y+1].obstacle){
            if(List[0]->gCost + 1 < nodes[x][y+1].gCost){
                nodes[x][y+1].gCost  = List[0]->gCost + 1;
                nodes[x][y+1].parent = parent;
                nodes[x][y+1].fCost  = nodes[x][y+1].gCost + nodes[x][y+1].hCost;
                if(!nodes[x][y+1].visited)
                    List.push_back(&nodes[x][y+1]);
                nodes[x][y+1].visited = true;
            }
        }
        List.erase(List.begin());
        //shell sort list, simple to implement but still pretty fast
        for(int gap = List.size() / 2; gap > 0; gap /= 2){
            for(int i = gap; i < List.size(); i++){
                for(int j = i; j >= gap; j -= gap){
                    if(List[j-gap]->fCost > List[j]->fCost){
                        Node* temp  = List[j-gap];
                        List[j-gap] = List[j];
                        List[j]     = temp;
                    }
                    else
                        break;
                }
            }
        }
    }
}
void colorPath(){
    /*
    for(int x = 0; x < GRID_WIDTH; x++){
        for(int y = 0; y < GRID_HEIGHT; y++){
            if(startNode != &nodes[x][y] && endNode != &nodes[x][y] && nodes[x][y].visited){
                boxes[x][y].setFillColor(sf::Color::Magenta);
            }
        }
    }
    */


    if(endNode->parent == nullptr)
        cout << "No path found" << endl;
    else{
        Node* parent = endNode->parent;
        while(parent != startNode){
            boxes[parent->x][parent->y].setFillColor(sf::Color::Blue);
            parent = parent->parent;
        }
        cout << "Path found" << endl;
    }
}
void resetGrid(){
    if(endNode->parent != nullptr){
        Node* parent = endNode->parent;
        while(parent != startNode){
            if(!parent->obstacle)
                boxes[parent->x][parent->y].setFillColor(sf::Color::White);
            parent = parent->parent;
        }
    }
    for(int x = 0; x < GRID_WIDTH; x ++){
        for(int y = 0; y < GRID_HEIGHT; y++){
            nodes[x][y].parent  = nullptr;
            nodes[x][y].gCost   = INT_MAX;
            nodes[x][y].visited = false;
        }
    }
    endNode  ->visited = true;
    startNode->visited = true;
    startNode->gCost   = 0;
}
void changeStart(int x, int y){
    startNode->gCost   = INT_MAX;
    startNode->visited = false;
    boxes[startNode->x][startNode->y].setFillColor(sf::Color::White);
    startNode = &nodes[x][y];
    startNode->x       = x;
    startNode->y       = y;
    startNode->gCost   = 0;
    startNode->visited = true;
    boxes[x][y].setFillColor(sf::Color::Green);
}
void changeEnd(int x, int y){
    endNode->visited = false;
    boxes[endNode->x][endNode->y].setFillColor(sf::Color::White);
    endNode = &nodes[x][y];
    endNode->x     = x;
    endNode->y     = y;
    endNode->visited = true;
    boxes[x][y].setFillColor(sf::Color::Red);
}
void calculateHeuristic(){
    for(int x = 0; x < GRID_WIDTH; x++)
        for(int y = 0; y < GRID_HEIGHT; y++)
            nodes[x][y].hCost = distance(x, y);
}
