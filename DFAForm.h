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


//DFA�Ľڵ㣬����ɽṹ�壬
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

//DFA�ı�
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

//DFA��Ԫ��һ�����DFA��Ԫ�����ߺ�����״ֱ̬�ӹ���
class DCell {
public:

    vector<DEdge> DEdges;

    DState startState{};
    DState endState{};

};


//�ж�����set�����Ƿ����
//����map�Ƚ�
bool isEqualset(const set<int>& s1,const set<int>& s2 ){
    std::map<std::set<int>,string> Map;
    Map[s1]="1";
    if(Map.find(s2)!=Map.end()) return true;
    else return false;

}

//����һ��DFA��Ԫ��һ��set���ϣ�������Ħ�-closure
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

DCell NFAToDFA(const Cell& cell){//todo �Ȳ����п�״̬����
    DCell newDCell;
    set<char> charSet;
    set<int> iniset;
    queue<set<int>> sets;//ÿһ��״̬���꼴�ӣ���������
    stack<set<int>> setstack;//���ÿһ���õ���״̬�������Ƚ�
    //�����������,��ȥ'#'
    for (auto e:cell.Edges) {
        if (e.transSymbol!='#') charSet.insert(e.transSymbol);
    }

    //һ��ʼ�Ħ�-closure
    iniset.insert(cell.startState.statename);
    iniset= getclosure(cell,iniset);//todo ��ʼ״̬
    DState startState{},endState{};
    startState.stateSet=iniset;
    sets.push(iniset);
    setstack.push(iniset);
    set<int> set1;
    //��sets�е�ÿ��һ�����α���
    do {

        set<int> setfront=sets.front();
        sets.pop();
        //��ÿ��״̬������������ַ�
        for (auto c:charSet) {
            for (auto e:cell.Edges) {
                //������   e.startState.statename-c->e.endState.statename ������һ����
                if((e.transSymbol==c)&&(setfront.find(e.startState.statename)!=setfront.end()))
                    set1.insert(e.endState.statename);
            }
            set1= getclosure(cell,set1);
            //(ע�⣬����õ��ıհ��ǿռ���������֮��
            if (!set1.empty()){
                //�����µı�
                DState state1(setfront);
                DState state2(set1);
                DEdge edge(state1,state2,c);
                newDCell.DEdges.push_back(edge);
                //�ж��Ƿ����µ�״̬
                if (isNewSet(setstack,set1)){
                    sets.push(set1);
                    setstack.push(set1);
                }
            }



            set1.clear();//ÿ�ζ�Ҫ���
        }


    }while (!sets.empty());

    //�����ʼ���ս�״̬
    //endState=setstack.top();//��̬----���󣡣��뵱Ȼ������������,��Ϊsetstack��ֻ��˳��ִ�еõ���һ������
    //�����һ�����������ĵ���̬�������ڽ��ж��ս�״̬�Ľ���
    for (auto e:newDCell.DEdges) {
        if (e.endState.stateSet.find(cell.endState.statename)!=e.endState.stateSet.end()){
            newDCell.endState=e.endState;
            break;
        }
    }
    newDCell.startState=startState;
    newDCell.startState.statename=1;
//    newDCell.endState.statename=setstack.size();//wrong!

    //����setstack�����һ������,����˳��״̬������
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

    //ȷ����̬�����
    for (auto e:newDCell.DEdges) {
        if (isEqualset(e.endState.stateSet,newDCell.endState.stateSet)){
            newDCell.endState.statename=e.endState.statename;
            break;
        }
    }

    //�������״̬��ȷ�������DCell��������Cell��endstate������Ϊ����״̬
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
    int nowState=dCell.startState.statename;//��̬
    for (char i : str) {
        flag=0;

        for (const auto& e:dCell.DEdges) {
            if (e.transSymbol==i&&e.startState.statename==nowState)//����statenameΨһ
            {
                nowState=e.endState.statename;
                flag=1;
                break;
            }
        }
        if (flag==0){
            cout<<"[error] �޷�������һ״̬"<<endl;
            break;
        }
    }
    if (nowState==dCell.endState.statename&&flag) cout<<"�ַ������ϸ��ķ�"<<endl;
    else cout<<"�ַ��������ϸ��ķ�"<<endl;
}

#endif //DFA_DFAFORM_H
