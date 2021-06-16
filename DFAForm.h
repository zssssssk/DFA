//
// Created by 86180 on 2021/6/14.
//

#ifndef DFA_DFAFORM_H
#define DFA_DFAFORM_H
#include <iostream>
#include <set>
#include <map>
#include "NFAForm.h"

using namespace std;


//DFA的节点，定义成结构体，
class DState{
public:
    int statename=0;
    set<int> stateSet;
public:
    DState(set<int> ss){

        this->stateSet=ss;
    }
    DState() = default;
};

//DFA的边
class DEdge{
public:
    DState startState{};
    DState endState{};
    char transSymbol;
public:
    DEdge(DState startState,DState endState,char transSymbol){
        this->startState=startState;
        this->endState=endState;
        this->transSymbol=transSymbol;
    }
};

//DFA单元，一个大的DFA单元由许多边和两个状态直接构成
class DCell {
public:

    vector<DEdge> DEdges;

    DState startState{};
    DState endState{};

};


//判断两个set集合是否相等
//借助map比较
bool isEqualset(const set<int>& s1,const set<int>& s2 ){
    std::map<std::set<int>,string> Map;
    Map[s1]="1";
    if(Map.find(s2)!=Map.end()) return true;
    else return false;

}

//给定一个DFA单元，一个set集合，求出它的ε-closure
set<int> getclosure(const Cell& cell,const set<int>& set){
    std::set<int> afterSet=set;
    std::set<int> beforeSet;

    do {
        beforeSet=afterSet;
        for (auto i:beforeSet) {
            for (auto e:cell.Edges) {
                if (e.startState.statename==i&&e.transSymbol=='#'){
                    afterSet.insert(e.endState.statename);
                }
            }
        }
    } while (!isEqualset(beforeSet,afterSet));

    return afterSet;
}

bool isNewSet(const stack<set<int>>& sets,const set<int>& sett){
    stack<set<int>> ss=sets;
    while(!ss.empty()){
        if (isEqualset(ss.top(),sett)) return false;
        ss.pop();
    }

    return true;
}

DCell NFAToDFA(const Cell& cell){//todo 先不管有空状态集合
    DCell newDCell;
    set<char> charSet;
    set<int> iniset;
    queue<set<int>> sets;//每一个状态用完即扔，用来遍历
    stack<set<int>> setstack;//存放每一个得到的状态，用来比较
    //所有输入符号,除去'#'
    for (auto e:cell.Edges) {
        if (e.transSymbol!='#') charSet.insert(e.transSymbol);
    }

    //一开始的ε-closure
    iniset.insert(cell.startState.statename);
    iniset= getclosure(cell,iniset);//todo 开始状态
    DState startState{},endState{};
    startState.stateSet=iniset;
    sets.push(iniset);
    setstack.push(iniset);
    set<int> set1;
    //对sets中的每个一个依次遍历
    do {

        set<int> setfront=sets.front();
        sets.pop();
        //对每个状态集合添加输入字符
        for (auto c:charSet) {
            for (auto e:cell.Edges) {
                //即存在   e.startState.statename-c->e.endState.statename 这样的一条边
                if((e.transSymbol==c)&&(setfront.find(e.startState.statename)!=setfront.end()))
                    set1.insert(e.endState.statename);
            }
            set1= getclosure(cell,set1);
            //(注意，如果得到的闭包是空集，则舍弃之）
            if (!set1.empty()){
                //加入新的边
                DState state1(setfront);
                DState state2(set1);
                DEdge edge(state1,state2,c);
                newDCell.DEdges.push_back(edge);
                //判断是否是新的状态
                if (isNewSet(setstack,set1)){
                    sets.push(set1);
                    setstack.push(set1);
                }
            }



            set1.clear();//每次都要清空
        }


    }while (!sets.empty());

    //加入初始、终结状态
    //endState=setstack.top();//终态----错误！！想当然了呜呜呜呜呜,因为setstack里只是顺序执行得到的一个集合
    //随便找一个符合条件的当终态，后面在进行多终结状态的接受
    for (auto e:newDCell.DEdges) {
        if (e.endState.stateSet.find(cell.endState.statename)!=e.endState.stateSet.end()){
            newDCell.endState=e.endState;
            break;
        }
    }
    newDCell.startState=startState;
    newDCell.startState.statename=1;
//    newDCell.endState.statename=setstack.size();//wrong!

    //发挥setstack的最后一点余热,按照顺序将状态重命名
    int i=setstack.size();
    while(!setstack.empty()){
        for (auto &e:newDCell.DEdges) {
            if (isEqualset(e.endState.stateSet,setstack.top())){
                e.endState.statename=i;
            }
            if (isEqualset(e.startState.stateSet,setstack.top())){
                e.startState.statename=i;
            }
        }
        i--;setstack.pop();
    }

    //确定终态的序号
    for (auto e:newDCell.DEdges) {
        if (isEqualset(e.endState.stateSet,newDCell.endState.stateSet)){
            newDCell.endState.statename=e.endState.statename;
            break;
        }
    }

    //多个接受状态的确定，如果DCell集合中有Cell的endstate，则视为接受状态
    for (auto &e:newDCell.DEdges) {
        if (e.endState.stateSet.find(cell.endState.statename)!=e.endState.stateSet.end()){
            e.endState.statename=newDCell.endState.statename;
        }
        if (e.startState.stateSet.find(cell.endState.statename)!=e.startState.stateSet.end()){
            e.startState.statename=newDCell.endState.statename;
        }

    }

    return newDCell;
}

void verityDFA(const DCell& dCell,const string& str){
    int flag;
    int nowState=dCell.startState.statename;//现态
    for (char i : str) {
        flag=0;

        for (const auto& e:dCell.DEdges) {
            if (e.transSymbol==i&&e.startState.statename==nowState)//反正statename唯一
            {
                nowState=e.endState.statename;
                flag=1;
                break;
            }
        }
        if (flag==0){
            cout<<"[error] 无法进入下一状态"<<endl;
            break;
        }
    }
    if (nowState==dCell.endState.statename&&flag) cout<<"字符串符合该文法"<<endl;
    else cout<<"字符串不符合该文法"<<endl;
}

#endif //DFA_DFAFORM_H
