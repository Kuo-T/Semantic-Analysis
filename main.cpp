#include<iostream>
#include<vector>
#include<string>
#include<cmath>

using namespace std;

int lex_flag = 1;

string orig_G[9] = {//原文法表达式
	"E->E+T", "E->E-T", "E->T", "T->T*F", "T->T/F", "T->F", "F->n.n", "F->(E)", "F->n"
};

char term_table[9] = {//终结符表
	'+', '-', '*', '/', '(', ')', '.', 'n', '$'
};

char non_term_table[3] = {//非终结符表
	'E', 'T', 'F'
};

string Action_Table[18][9] = {//分析表ACTION部分
	{"", "", "", "", "S5", "", "", "S4", ""},
	{"S6", "S7", "", "", "", "", "", "", "ACC"},
	{"R3", "R3", "S8", "S9", "", "R3", "", "", "R3"},
	{"R6", "R6", "R6", "R6", "", "R6", "", "", "R6"},
	{"R9", "R9", "R9", "R9", "", "R9", "S10", "", "R9"},
	{"", "", "", "", "S5", "", "", "S4", ""},
	{"", "", "", "", "S5", "", "", "S4", ""},
	{"", "", "", "", "S5", "", "", "S4", ""},
	{"", "", "", "", "S5", "", "", "S4", ""},
	{"", "", "", "", "S5", "", "", "S4", ""},
	{"", "", "", "", "", "", "", "S16", ""},
	{"S6", "S7", "", "", "", "S17", "", "", ""},
	{"R1", "R1", "S8", "S9", "", "R1", "", "", "R1"},
	{"R2", "R2", "S8", "S9", "", "R2", "", "", "R2"},
	{"R4", "R4", "R4", "R4", "", "R4", "", "", "R4"},
	{"R5", "R5", "R5", "R5", "", "R5", "", "", "R5"},
	{"R7", "R7", "R7", "R7", "", "R7", "", "", "R7"},
	{"R8", "R8", "R8", "R8", "", "R8", "", "", "R8"}
};

int Goto_Table[18][3] = {//分析表GOTO部分
	{ 1, 2, 3 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ 11, 2, 3 },
	{ -1, 12, 3 },
	{ -1, 13, 3 },
	{ -1, -1, 14 },
	{ -1, -1, 15 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 }
};

int get_term_indx( char term )//终结符位置返回
{
	for ( int i = 0; i < 9; i++ ){
		if ( term_table[i] == term )
			return i;
	}
}

int get_nonterm_indx( int nonterm )//非终结符位置返回
{
	for ( int i = 0; i < 3; i++ ){
		if ( non_term_table[i] == nonterm )
			return i;
	}
}

void lex_error(int err_kind)//词法分析过程错误信息输出
{
	lex_flag = 0;
	if ( err_kind == 1 ){
		cout << "Invalid token." << endl;
	}
	else if ( err_kind == 2 ){
		cout << "Too much decimal number." << endl;
	}
}

void grammar_error()//语法分析过程错误信息输出
{
	cout << "Unaccepted sentence."<< endl;
}

double calculate( int ptr, int num, vector<double> val, vector<char> type )//LR分析中计算表达式的值
{
	double result;
	int temp;
	int length = 0;
	int d = 10;
	switch(num){
		case 1:
			result = val[ptr] + val[ptr-2];
			break;
		case 2:
			result = val[ptr-2] - val[ptr];
			break;
		case 3:
			result = val[ptr];
			break;
		case 4:
			if ( type[ptr] == 'i' && type[ptr-2] == 'i')
				result = (int)((int)val[ptr] * (int)val[ptr-2]);
			else
				result = val[ptr] * val[ptr-2];
			break;
		case 5:
			if ( type[ptr] == 'i' && type[ptr-2] == 'i')
				result = (int)((int)val[ptr-2] / (int)val[ptr]);
			else{
				result = val[ptr-2] / val[ptr];
			}
			break;
		case 6:
			result = val[ptr];
			break;
		case 7:
			temp = (int)val[ptr];
			while( temp != 0 ){
				temp = temp/d;
				length++;
			}
			result = val[ptr-2] + double(val[ptr])/pow((double)10, length);
			break;
		case 8:
			result = val[ptr-1];
			break;
		case 9:
			result = val[ptr];
			break;
	}
	return result;
}

char kind ( int ptr, int num, vector<char> type )//LR分析中计算表达式类型
{
	char result;
	switch(num){
		case 1:
			if ( type[ptr] == 'i' && type[ptr-2] == 'i' )
				result = 'i';
			else 
				result = 'r';
			break;
		case 2:
			if ( type[ptr] == 'i' && type[ptr-2] == 'i' )
				result = 'i';
			else 
				result = 'r';
			break;
		case 3:
			result = type[ptr];
			break;
		case 4:
			if ( type[ptr] == 'i' && type[ptr-2] == 'i' )
				result = 'i';
			else 
				result = 'r';
			break;
		case 5:
			if ( type[ptr] == 'i' && type[ptr-2] == 'i' )
				result = 'i';
			else 
				result = 'r';
			break;
		case 6:
			result = type[ptr];
			break;
		case 7:
			result = 'r';
			break;
		case 8:
			result = type[ptr-1];
			break;
		case 9:
			result = 'i';
			break;
	}

	return result;
}
int main()
{
	string math_sentence;
	cout << "Please input the math sentence:"<< endl;
	cin >> math_sentence;
	string buf;
	vector<char> token;//保存词法分析中每个识别的字符
	vector<char> bufa;//保存词法分析后的表达式结果
	vector<int> buf_val;//保存词法分析中数字的值
	vector<int> state_stack;//LR分析的状态栈
	vector<double> val_stack;//LR分析的数值栈
	vector<char> type_stack;//LR分析的类型栈
	int ip = 0;
	int top1 = 0;
	int p_num = 0;
	int S;
	char a;
	buf = math_sentence + "$";

	cout << "Analysing process"<< endl;
	int i = 0;
	int state = 0;
	int n1 = 0, n2 = 0;
	int point = 0;
	int start = 0;
	while( i < buf.size() && lex_flag == 1 ){//对算术表达式进行词法分析
		switch(state){
			case 0:
				start = i;
				if ( buf[i] >= '0' && buf[i] <= '9' ){//整数部分
					bufa.push_back('n');
					token.push_back(buf[i]);
					state = 1;
				}
				else {
					if ( buf[i] == '+' || buf[i] == '-' || buf[i] == '*' || buf[i] == '/' || buf[i] == '(' || buf[i] == ')' || buf[i] == '$')
						bufa.push_back(buf[i]);
					else{
						lex_error(1);
						cout << "Error location: " << i+1 << endl;
					}
					state = 0;
				}
				break;
			case 1:
				if ( buf[i] >= '0' && buf[i] <= '9' ){
					token.push_back(buf[i]);
					state = 1;
				}
				else if ( buf[i] == '.' ){
					bufa.push_back('.');
					point = i - start;
					state = 2;
				}
				else{
					string num;
					num.assign(token.begin(), token.end());
					n1 = atoi(num.c_str());
					buf_val.push_back(n1);
					i--;
					token.clear();
					state = 0;
				}
				break;
			case 2:
				if ( buf[i] >= '0' && buf[i] <= '9' ){//小数部分
					bufa.push_back('n');
					token.push_back(buf[i]);
					state = 3;
				}
				else{
					i--;
					state = 0;
				}
				break;
			case 3:
				if ( buf[i] >= '0' && buf[i] <= '9' ){
					token.push_back(buf[i]);
					state = 3;
				}
				else{
					vector<char> num1;
					vector<char> num2;
					string num_1;
					string num_2;
					
					for ( int j = 0; j < point; j++ )
						num1.push_back(token[j]);
					for ( int j = point; j < token.size(); j++ )
						num2.push_back(token[j]);
					num_1.assign(num1.begin(), num1.end());
					num_2.assign(num2.begin(), num2.end());
					n1 = atoi(num_1.c_str());
					n2 = atoi(num_2.c_str());
					buf_val.push_back(n1);
					buf_val.push_back(n2);
					num1.clear();
					num2.clear();
					num_1 = "";
					num_2 = "";
					token.clear();
					i--;
					point = 0;
					state = 0;
				}
				break;
		}
		i++;
	}
	cout << endl;
	for ( int i = 0; i < bufa.size(); i++ ){
		cout << bufa[i];
	}

	state_stack.push_back(0);
	val_stack.push_back('-');
	type_stack.push_back('_');
	do{//对词法分析后的表达式进行LR语法分析
		S = state_stack[top1];
		a = bufa[ip];
		if ( Action_Table[S][get_term_indx(a)] == "" ){
			grammar_error();
			break;
		}
		else if ( Action_Table[S][get_term_indx(a)].at(0) == 'S' ){//移进动作
			cout << "State:  ";
			for ( int i = 0; i < state_stack.size(); i++ ){
				cout << state_stack[i]<< '|';
			}
			cout << '\t';
			for ( int k = ip; k < bufa.size(); k++ ){
				cout << bufa[k];
			}
			cout << '\t';
			string entry = Action_Table[S][get_term_indx(a)];
			string next_state = entry.substr(1);
			int S1 = atoi(next_state.c_str());
			cout << "Shift "<< S1;
			cout << endl;
			cout << endl;
			state_stack.push_back(S1);
			if ( bufa[ip] == 'n' ){
				val_stack.push_back(buf_val[p_num]);
				type_stack.push_back('i');
				p_num++;
			}
			else{
				val_stack.push_back(0);
				type_stack.push_back('#');
			}
			top1++;
			ip++;
		}
		else if ( Action_Table[S][get_term_indx(a)].at(0) == 'R' ){//规约动作
			cout << "State:  ";
			for ( int i = 0; i < state_stack.size(); i++ ){
				cout << state_stack[i]<< '|';
			}
			cout << '\t';
			for ( int k = ip; k < bufa.size(); k++ ){
				cout << bufa[k];
			}
			cout << '\t';
			string entry = Action_Table[S][get_term_indx(a)];
			string regress_sntns = entry.substr(1);
			int R = atoi(regress_sntns .c_str());
			string pop_symble = orig_G[R-1].substr(3);
			int pop_length = pop_symble.length();
			cout << "Reduce by "<< orig_G[R-1];
			cout << endl;
			cout << endl;
			double result;
			char sen_type;
			result = calculate(top1, R, val_stack, type_stack);
			sen_type = kind(top1, R, type_stack);
			for ( int i = 0; i < pop_length; i++ ){
				state_stack.pop_back();
				val_stack.pop_back();
				type_stack.pop_back();
				top1--;
			}
			//
			int S2 = state_stack[top1];
			char next_symble = orig_G[R-1].at(0);
			int newS = Goto_Table[S2][get_nonterm_indx(next_symble)];
			state_stack.push_back(newS);
			val_stack.push_back(result);
			type_stack.push_back(sen_type);
			top1++;
		}
		else if ( Action_Table[S][get_term_indx(a)] == "ACC" ){
			cout << "State:  ";
			for ( int i = 0; i < state_stack.size(); i++ ){
				cout << state_stack[i] << '|';
			}
			cout << '\t';
			for ( int k = ip; k < bufa.size(); k++ ){
				cout << bufa[k];
			}
			cout << '\t';
			cout << "ACC";
			cout << endl;
			break;
		}
	}while(1);
	cout << "The result of the math sentence is: "<< val_stack[top1]<< endl;
	cout << "The type of the math sentence is: " << type_stack[top1]<< endl;
	system("pause");
	return 0;
}