//
// Created by 86180 on 2021/6/13.
//

#ifndef DFA_SUFFIXFORM_H
#define DFA_SUFFIXFORM_H



#include <string>
#include <stack>
using namespace std;





/*推断符号间的优先关系函数
*不战而屈人之兵
*c1栈内的算符。c2栈外的算符
*/
int Judge(char c1,char c2)
{
    int a1,a2;
    if('|'==c1) a1 = 3;
    if('.'==c1) a1 = 5;
    if('*'==c1) a1 = 7;
    if('('==c1) a1 = 1;
    if(')'==c1) a1 = 9;
    if('#'==c1) a1 = 0;

    if('|'==c2) a2 = 2;
    if('.'==c2) a2 = 4;
    if('*'==c2) a2 = 6;
    if('('==c2) a2 = 8;
    if(')'==c2) a2 = 1;
    if('#'==c2) a2 = 0;
    if(a1>a2) return 1;//栈外优先级小
    else if(a1==a2) return 0;// 直接丢弃
    else return -1;//栈外优先级大
}


//将正则表达式转为后缀表达式
string suffixForm(string& str)
{
    string s = "*.|()#";
    //给表达式字符串str加入'#'结束标识符
    str.append(1,'#');
    stack<char> OPTR;//运算符栈

    string suffixFormString;
    int a = -1;
    //先将#符号入栈
    OPTR.push('#');
    while(true)
    {
        int b = a+1;
        a = str.find_first_of(s,b);//*在一个字符串s中查找另一个字符串str,如果str中含有s中的任何字符，则就会查找成功(返回任何一个在str中首次出现的位置），而find则不同
        if(a==string::npos) break;//*string::npos如果作为一个返回值表示没有找到匹配项
        if(a!=b)
        {
            string ss(str,b,a-b);//非常不错的拷贝构造函数
            suffixFormString.append(ss);
        }
        //运算符优先级比较
        int ju = Judge(OPTR.top(),str[a]);
        if(-1==ju)//栈外优先级大直接入栈
        {
            OPTR.push(str[a]);
        }
        else if(0==ju)//栈内外优先级相等则出栈
        {
            OPTR.pop();
        }
        else//栈内优先级大,出栈进行运算
        {
            while(Judge(OPTR.top(),str[a])==1){
                suffixFormString.append(1,OPTR.top());
                OPTR.pop();
            }


            a--;
        }
    }
    //删除表达式最后的'#'结束标识符
    str.erase(str.length()-1,1);//*erase(pos,n),删除从下标pos开始的n个字符
    return suffixFormString;
}

bool isLetter(char a){
    if((a>='a'&&a<='z')||(a>='A'&&a<='Z')){
        return true;
    } else return false;
}

//将缺省的符号加上去
//共四种情况需要加乘号（.)  1.ab 2.a( 3.)a 4.*a
void regulartoRplus(string& str){
    for (int i = 0; i < str.length()-1; ++i) {
        if(isLetter(str[i])&& (isLetter(str[i+1])||(str[i+1]=='('))){
            str.insert(i+1,1,'.');
        }
        if ((str[i]==')'||str[i]=='*')&& isLetter(str[i+1])){
            str.insert(i+1,1,'.');
        }
    }

}


#endif //DFA_SUFFIXFORM_H
