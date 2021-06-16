#include <iostream>
#include <fstream>
#include "suffixForm.h"
#include "DFAForm.h"
#include<string>

using namespace std;
void coutSet(const set<int>& a){
    cout<<"{";
    for (auto i:a) {
        cout<<i<<" ";
    }
    cout<<"}";
}
int main() {
    string regularExpression;
    string suffixFormExpression;
    string t="��";
    ifstream in(R"(D:\Code Design\CompilerPrinciple\DFA\test.txt)",ios::in);
    while(getline(in,regularExpression)){
        cout<<"����:"<<regularExpression<<endl;
        regulartoRplus(regularExpression);
        suffixFormExpression=suffixForm(regularExpression);

        cout<<"suffixForm:"<<suffixFormExpression<<endl;
        //NFA
        cout<<"NFA:"<<endl;
        Cell cell=suffixToDFA(suffixFormExpression);
        for (const auto& e:cell.Edges ) {
            cout<< e.startState.statename<<" -"<<e.transSymbol<<"->"<<e.endState.statename<<endl;
        }
        DCell dCell=NFAToDFA(cell);
        //״̬ת����
        cout<<"״̬ת����:"<<endl;
        for (const auto& e:dCell.DEdges) {
            coutSet(e.startState.stateSet);
            cout<<" -"<<e.transSymbol<<"->";
            coutSet(e.endState.stateSet);
            cout<<endl;
        }
        //DFA
        cout<<"DFA:"<<endl;
        for (const auto& e:dCell.DEdges) {
            cout<< e.startState.statename<<" -"<<e.transSymbol<<"->"<<e.endState.statename<<endl;
        }
        cout<<"��������֤DFA���ַ�������(zs ����):";
        while (cin>>t){
            if(t=="zs") break;
            verityDFA(dCell,t);
        }

    }
    in.close();

    return 0;
}
