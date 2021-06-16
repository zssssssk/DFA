//
// Created by 86180 on 2021/6/13.
//

#ifndef DFA_NFAFORM_H
#define DFA_NFAFORM_H
#include <string>
#include <stack>
#include <vector>
#include <queue>

using namespace std;

//NFA的节点，定义成结构体，便于以后扩展
class State{
public:
    int statename;

};

//NFA的边，空转换符用'#'表示
class Edge{
public:
    State startState{};
    State endState{};
    char transSymbol;
public:
    Edge(State startState,State endState,char transSymbol){
        this->startState=startState;
        this->endState=endState;
        this->transSymbol=transSymbol;
    }
};

//NFA单元，一个大的NFA单元由许多小单元通过规则拼接起来(只需要如下四个属性)
class Cell {
public:

    vector<Edge> Edges;

    State startState{};
    State endState{};
    static int stateNum;//确保状态不会重复


public:
    //构造函数，建立起单个字符的NFA
    explicit Cell(char a) {
        stateNum+=2;
        startState.statename=stateNum-1;
        endState.statename=stateNum;
        Edge x(startState,endState,a);
        Edges.push_back(x);
    }
    static void changeStateNum(int i){
        stateNum+=i;
    }
    Cell()= default;
};

int Cell::stateNum=0;

//处理 a|b  （加四条边，两个状态）
Cell do_or(const Cell& leftCell,const Cell& rightCell){
    Cell newCell;
    Cell::changeStateNum(2);
    //获得新的状态节点
    State startState{},endState{};
    startState.statename=Cell::stateNum-1;
    endState.statename=Cell::stateNum;
    //构建边
    Edge e1(startState,leftCell.startState,'#');
    Edge e2(startState,rightCell.startState,'#');
    Edge e3(leftCell.endState,endState,'#');
    Edge e4(rightCell.endState,endState,'#');
    //先将leftCell,rightCell的边全部复制到newCell中
    for (auto e:leftCell.Edges){
        newCell.Edges.push_back(e);
    }
    for (auto e:rightCell.Edges){
        newCell.Edges.push_back(e);
    }
    //
    newCell.Edges.push_back(e1);
    newCell.Edges.push_back(e2);
    newCell.Edges.push_back(e3);
    newCell.Edges.push_back(e4);

    //构建newCell的起始状态和初始状态
    newCell.startState=startState;
    newCell.endState=endState;

    return newCell;

}

//处理 ab （合并状态）
Cell do_and(const Cell& leftCell,Cell rightCell){
    Cell newCell;
    //将leftCell的接受状态与rightCell的开始状态合并，其余不变
    for (auto &e:rightCell.Edges) {//注意，这里要加上&才能改变值，不然只读
        if (e.startState.statename==rightCell.startState.statename){
            e.startState.statename=leftCell.endState.statename;
        }
    }
    rightCell.startState.statename=leftCell.endState.statename;
    for (auto e:leftCell.Edges){
        newCell.Edges.push_back(e);
    }
    for (auto e:rightCell.Edges){
        newCell.Edges.push_back(e);
    }

    newCell.startState=leftCell.startState;
    newCell.endState=rightCell.endState;

    return newCell;
}

//do a*  (加四条边，两个状态）
Cell do_star(const Cell& cell){
    Cell newCell;
    Cell::changeStateNum(2);
    //获得新的状态节点
    State startState{},endState{};
    startState.statename=Cell::stateNum-1;
    endState.statename=Cell::stateNum;
    //构建边
    Edge e1(startState,endState,'#');
    Edge e2(startState,cell.startState,'#');
    Edge e3(cell.endState,cell.startState,'#');
    Edge e4(cell.endState,endState,'#');

    //先将cell的边全部复制到newCell中
    for (auto e:cell.Edges) {
        newCell.Edges.push_back(e);
    }
    //将新构建的四条边加入newCell
    newCell.Edges.push_back(e1);
    newCell.Edges.push_back(e2);
    newCell.Edges.push_back(e3);
    newCell.Edges.push_back(e4);

    //构建newCell的起始状态和初始状态
    newCell.startState=startState;
    newCell.endState=endState;
    return newCell;
}



Cell suffixToDFA(const string& str){
    Cell::stateNum=0;
    string s="*.|";

    stack<Cell> letterOperator;//因为是NFA的操作，所以栈里放NFA的单元

    for (char c : str) {
        if (isLetter(c)) {
            Cell cell1(c);
            letterOperator.push(cell1);
        }
        else{

                if (c == '*') {

                    Cell cell1 = do_star(letterOperator.top());
                    letterOperator.pop();
                    letterOperator.push(cell1);

                } else if (c == '.') {
                    if (letterOperator.empty()) {
                        cout << "字母不够!" << endl;
                        break;
                    }
                    Cell cell2=letterOperator.top();
                    letterOperator.pop();
                    Cell cell1=letterOperator.top();
                    letterOperator.pop();
                    letterOperator.push(do_and(cell1, cell2));

                } else if (c == '|') {
                    if (letterOperator.empty()) {
                        cout << "letter is not enough!" << endl;
                        break;
                    }
                    Cell cell2=letterOperator.top();
                    letterOperator.pop();
                    Cell cell1=letterOperator.top();
                    letterOperator.pop();
                    letterOperator.push(do_or(cell1, cell2));

                } else {
                    cout << "正则表达式有问题!" << endl;
                    break;
                }

            }

        }




    return letterOperator.top();
}



#endif //DFA_NFAFORM_H
