#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;


struct  CMOS
{
	string D;
	string G;
	string S;

};

//check start & end if needed
struct block
{
	vector <CMOS> chain;

};

int mos_c = 1;
int wire = 1;

bool last_block(string exp, int start)
{
	for (int i = start; i < exp.length(); i++)
		if (exp[i] == '&')
			return false;

	return true;
}

bool validate_input(vector<string> exp) {

	for (int i = 0; i < exp.size(); i++) {
		for (int j = 2; j < exp[i].length(); j++) {

			if (exp[i][j] == exp[i][0])
				return false;

			if (exp[i][j] != '|' && exp[i][j] != '&' && exp[i][j] != '\'' && !(isalpha(exp[i][j])))
				return false;
		}
	}

	return true;

}



void inverter(string exp) {


	for (int i = 2; i < exp.length(); i++) {
		if (isalpha(exp[i])) {
			if (exp[i + 1] != '\'')
			{

				cout << "M" << mos_c << " " << exp[i] << "' " << exp[i] << " vdd vdd PMOS" << endl;
				mos_c++;
				cout << "M" << mos_c << " " << exp[i] << "' " << exp[i] << " 0 0 NMOS" << endl;
				mos_c++;
			}

		}
	}
	cout << endl;
}

void PDN(string exp) {
	for (int i = 2; i < exp.length(); i++) {
		if (!isalpha(exp[i])) {
			if (exp[i] == '&') {
				exp.erase(i, 1);
				exp.insert(i, "|");
			}
			else if (exp[i] == '|') {
				exp.erase(i, 1);
				exp.insert(i, "&");
			}
		}
	}

	block block_parallel;
	vector <block> circuit;
	string end(1,exp[0]);

	for (int i = 2; i < exp.length(); i++)
	{
		CMOS temp;
		if (isalpha(exp[i]))
		{
			if (exp[i + 1] == '\'')
			{
				temp.G = exp[i];
				temp.G += "'";
				if (i == 2)
					temp.D = exp[0];
				else if (!block_parallel.chain.empty())
				{
					temp.D = block_parallel.chain.back().D;
				}
				else
				{
					temp.D = "w";
					temp.D += to_string(wire);
					
				}

				if (last_block(exp, i + 2))
				{
					temp.S = "0";
					block_parallel.chain.push_back(temp);
				}
				else if (!block_parallel.chain.empty())
				{
					temp.S = block_parallel.chain.back().S;
					block_parallel.chain.push_back(temp);
				}
				else
				{

					temp.S = "w";
					temp.S += to_string(wire);
					block_parallel.chain.push_back(temp);
				}
				
				if (exp[i + 2] == '&')
				{
					circuit.push_back(block_parallel);
					block_parallel.chain.clear();
				}
			}
			else 
			{
				temp.G = exp[i];
				if (i == 2)
					temp.D = exp[0];
				else if (!block_parallel.chain.empty())
				{
					temp.D = block_parallel.chain.back().D;
				}
				else
				{
					temp.D = "w";
					temp.D +=to_string( wire);
					wire++;
				}

				if (last_block(exp, i + 1))
				{
					temp.S = "0";
					block_parallel.chain.push_back(temp);
				}
				else if (!block_parallel.chain.empty())
				{
					temp.S = block_parallel.chain.back().S;
					block_parallel.chain.push_back(temp);
				}
				else
				{
					wire++;
					temp.S = "w";
					temp.S += to_string( wire);
					block_parallel.chain.push_back(temp);
				}

				if (exp[i + 1] == '&')
				{
					circuit.push_back(block_parallel);
					block_parallel.chain.clear();
				}
			}
				

		}
	}
	circuit.push_back(block_parallel);

	for (int i = 0; i < circuit.size(); i++) {
		for (int j = 0; j < circuit[i].chain.size(); j++) {
			cout << "M" << mos_c << " " << circuit[i].chain[j].D << " " << circuit[i].chain[j].G << " " << circuit[i].chain[j].S
				<< " " << circuit[i].chain[j].S << " NMOS" << endl;

			mos_c++;
		}

	}
	cout << endl;
}


void PUN(string exp) {


	for (int i = 2; i < exp.length(); i++) {
		if (isalpha(exp[i])) {
			if (exp[i + 1] == '\'') {
				exp.erase(i + 1, 1);
			}
			else {
				exp.insert(i + 1, "'");
			}
		}
	}


	block block_series;
	vector <block> circuit;
	
	for (int i = 2; i < exp.length(); i++) {

		if (isalpha(exp[i])) {
			CMOS tmp_pmos;

			if (block_series.chain.empty()) {
				tmp_pmos.S = "vdd";
			}
			else {
				tmp_pmos.S = block_series.chain.back().D;
			}

			if (exp[i + 1] == '\'') {


				tmp_pmos.G = exp[i];
				tmp_pmos.G += "'";//gate is equal to char

				if (i + 1 != exp.length() - 1) {

					if (exp[i + 2] != '|')  // if it is not the last char segmentation fault
					{
						//if it is not the last char, connect to a wire to be connected to other char
						tmp_pmos.D = "w";
						tmp_pmos.D += to_string(wire);
						wire++;
						block_series.chain.push_back(tmp_pmos);


					}
					else {
						tmp_pmos.D = exp[0];
						block_series.chain.push_back(tmp_pmos);
						circuit.push_back(block_series);
						block_series.chain.clear();

					}

				}
				else {

					tmp_pmos.D = exp[0]; //if it is the last char connect to the output
					block_series.chain.push_back(tmp_pmos);
					circuit.push_back(block_series);

				}

			}
			else {

				tmp_pmos.G = exp[i];  //gate is equal to char

				if (i != exp.length() - 1) {

					if (exp[i + 1] != '|')  // if it is not the last char segmentation fault
					{
						//if it is not the last char, connect to a wire to be connected to other char
						tmp_pmos.D = "w";
						tmp_pmos.D += to_string(wire);
						wire++;
						block_series.chain.push_back(tmp_pmos);


					}
					else {
						tmp_pmos.D = exp[0];
						block_series.chain.push_back(tmp_pmos);
						circuit.push_back(block_series);
						block_series.chain.clear();

					}

				}
				else {

					tmp_pmos.D = exp[0]; //if it is the last char connect to the output
					block_series.chain.push_back(tmp_pmos);
					circuit.push_back(block_series);

				}
			}


		}


	}





	for (int i = 0; i < circuit.size(); i++) {
		for (int j = 0; j < circuit[i].chain.size(); j++) {
			cout << "M" << mos_c << " " << circuit[i].chain[j].D << " " << circuit[i].chain[j].G << " " << circuit[i].chain[j].S
				<< " " << circuit[i].chain[j].S << " PMOS" << endl;

			mos_c++;
		}

	}

	cout << endl;
	PDN(exp);
}





// check if we can have Y' input
int main() {

	string input;

	cout << "Please Enter an Expression " << endl;
	cin >> input;

	stringstream ss(input);
	string tmp;
	vector<string> exp;

	while (getline(ss, tmp, ';')) {
		exp.push_back(tmp);
	}

	if (!validate_input(exp)) {
		cout << "You entered an invalid input " << endl;
		return 0;
	}


	for (int i = 0; i < exp.size(); i++) {
		mos_c = 1;
		cout << endl;
		cout << "Expression: " << exp[i] << endl;
		inverter(exp[i]);
		PUN(exp[i]);
		
	}



	return 0;
}