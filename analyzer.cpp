#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann\json.hpp>
#include <string>
#include <sstream>
#include <vector>


using namespace std;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

typedef chrono::high_resolution_clock Time;
typedef chrono::milliseconds ms;
typedef chrono::microseconds us;
typedef chrono::duration<float> fsec;

ifstream infile_if_file_exists(string filepath) {
	ifstream infile_object;
	infile_object.open(filepath);
	if (!infile_object)
		throw std::runtime_error("FileNotFoundError in infile_if_file_exists()");
	return infile_object;
}

ifstream effortTableJson = infile_if_file_exists("keyboard_effort_table.json");
json EffortTable = json::parse(effortTableJson);
ifstream monogramJson = infile_if_file_exists("C:/Users/yezhi/Desktop/COMP 1011/machine learning/keyboard_analyzer/KeyboardAnalyzerSolution/x64/Release/n1gram_frequency.json");
json Monogram = json::parse(monogramJson);
ifstream bigramJson = infile_if_file_exists("n2gram_frequency.json");
json Bigram = json::parse(bigramJson);
ifstream trigramJson = infile_if_file_exists("n3gram_frequency.json");
json Trigram = json::parse(trigramJson);
ifstream constraintsJson = infile_if_file_exists("config.json");
ordered_json Constraints = ordered_json::parse(constraintsJson);
ifstream cutoffScoreJson = infile_if_file_exists("cutoff_score.json");
json CutoffScore = json::parse(cutoffScoreJson);


class Keyboard {
public:
	string KeyboardLayout = "";
	short KeyboardMap[30][2] = {};
	// -- Monogram stats
	double effort = 0.0;
	double hand_usage[2] = {};
	double hand_diff = 0.0;
	// -- Bigram stats
	double sfb = 0.0;
	double lsb = 0.0;
	double intwist = 0.0;
	double outtwist = 0.0;
	double outtwist_ratio = 0.0;
	double total_twists = 0.0;
	// -- Trigram stats
	double alternate = 0.0;
	double inroll = 0.0;
	double outroll = 0.0;
	double total_rolls = 0.0;
	double inroll_ratio = 0.0;
	double onehand = 0.0;
	double redirect = 0.0;

	void make_keyboard(string& keys) {
		if (keys.length() != 30) {
			throw runtime_error("Expected 30 char for keys");
			exit(1);
		}
		this->KeyboardLayout = keys;
		for (int i = 0; i < 30; i++) {
			this->KeyboardMap[to_keyboard_loc(KeyboardLayout[i])][0] = i / 10;
			this->KeyboardMap[to_keyboard_loc(KeyboardLayout[i])][1] = i % 10;
		}
	}

	void pretty_print() {
		string keys_pretty_print = "";
		for (int i = 0; i < 30; i++) {
			keys_pretty_print += this->KeyboardLayout[i];
			keys_pretty_print += ' ';
			if (i % 10 == 9) {keys_pretty_print += '\n';}
		}
		std::cout << "#   Keyboard Layout   #\n"
				  << "-----------------------\n"
				  << keys_pretty_print << '\n'
				  << "# Keyboard Statistics #\n"
				  << "-----------------------\n"
				  << "Effort : ";
		if (this->effort) { printf("%.5f\n", this->effort); }
		else { std::cout << '\n'; }
		std::cout << "Hand usage : ";
		if (this->hand_usage[0]) {
			printf("%.3f | %.3f\n", this->hand_usage[0], this->hand_usage[1]);
		}
		else { std::cout << '\n'; }
		std::cout << "Hand difference : ";
		if (this->hand_diff) { printf("%.3f\n", this->hand_diff); }
		else { std::cout << '\n'; }
		std::cout << "Sfb  : ";
		if (this->sfb) { printf("%.5f%%\n", this->sfb); }
		else { std::cout << '\n'; }
		std::cout << "Lsb  : ";
		if (this->lsb) { printf("%.5f%%\n", this->lsb); }
		else { std::cout << '\n'; }
		std::cout << "In/Out Twists  : ";
		if (this->intwist) { printf("%.5f%% | %.5f%%\n", this->intwist, this->outtwist); }
		else { std::cout << '\n'; }
		std::cout << "Total Twists : ";
		if (this->total_twists) { printf("%.5f%%\n", this->total_twists); }
		else { std::cout << '\n'; }
		std::cout << "Outtwist Ratio : ";
		if (this->outtwist_ratio) { printf("%.5f\n", this->outtwist_ratio); }
		else { std::cout << '\n'; }
		std::cout << "Alternates : ";
		if (this->alternate) { printf("%.5f%%\n", this->alternate); }
		else { std::cout << '\n'; }
		std::cout << "In/Out Rolls : ";
		if (this->inroll) { printf("%.5f%% | %.5f%%\n", this->inroll, this->outroll); }
		else { std::cout << '\n'; }
		std::cout << "Total Rolls  : ";
		if (this->total_rolls) { printf("%.5f%%\n", this->total_rolls); }
		else { std::cout << '\n'; }
		std::cout << "Inroll Ratio : ";
		if (this->inroll_ratio) { printf("%.5f\n", this->inroll_ratio); }
		else { std::cout << '\n'; }
		std::cout << "Onehand  : ";
		if (this->onehand) { printf("%.5f%%\n", this->onehand); }
		else { std::cout << '\n'; }
		std::cout << "Redirects  : ";
		if (this->redirect) { printf("%.5f%%\n", this->redirect); }
		else { std::cout << "\n"; }
	}

	/*
	Gets the keyboard stats of a given keyboard layout
	*/
	void get_stats() {
		this->get_monogram_stats();
		this->get_bigram_stats();
		this->get_trigram_stats();
	}

	/*
	Writes the keyboard stats to a text file
	*/
	void write_stats(ofstream &file) {
		file << "Keyboard Layout: " << this->KeyboardLayout << '\n'
			<< "Effort: " << this->effort << '\n'
			<< "Hand usage: " << this->hand_usage[0] << " | " << this->hand_usage[1] << '\n'
			<< "Hand difference: " << this->hand_diff << '\n'
			<< "Sfb: " << this->sfb << '\n'
			<< "Lsb: " << this->lsb << '\n'
			<< "In/OutTwists: " << this->intwist << " | " << "OutTwists: " << this->outtwist << '\n'
			<< "TotalTwists: " << this->total_twists << '\n'
			<< "OuttwistRatio: " << this->outtwist_ratio << '\n'
			<< "Alternates: " << this->alternate << '\n'
			<< "In/OutRolls: " << this->inroll << " | " << this->outroll << '\n'
			<< "TotalRolls: " << this->total_rolls << '\n'
			<< "InrollRatio: " << this->inroll_ratio << '\n'
			<< "Onehand: " << this->onehand << '\n'
			<< "Redirects: " << this->redirect << '\n';
	}

	short to_keyboard_loc(char key) {
		if (97 <= key && key < 123)
			return key - 97;
		switch (key) {
		case ';':
			return 26;
		case ',':
			return 27;
		case '.':
			return 28;
		case '\'':
			return 29;
		}
	}

	/*
	Gets the total typing effort of the whole keyboard and for individual fingers
	*/
	void get_monogram_stats() {
		double letter_freq;
		double letter_effort;
		
		for (int i = 0; i < 30; i++) {
			string key = "";
			key += this->KeyboardLayout[i];
			letter_freq = Monogram[key];
			
			letter_effort = EffortTable["key" + to_string(i)];
			this->effort += letter_freq * letter_effort;
			short col0;
			col0 = this->KeyboardMap[to_keyboard_loc(this->KeyboardLayout[i])][1];
			// Left hand [0] is col 0~4, Right hand [1] is col 5~9
			this->hand_usage[col0 >= 5] += (double)letter_freq;
		}
		this->hand_diff = abs(this->hand_usage[0] - this->hand_usage[1]);
	}

	/*
	Sfb (Same finger bigram): pressing two keys in succession with the same finger
	Lsb (Lateral stretch bigram): a bigram typed with adjacent fingers, but that requires
	a lateral stretch since the two letters forming the bigram are far apart.
	Twist (wrist Twisting bigram): a bigram requires twisting the wrist, either inwards or outwards
	*/
	void get_bigram_stats() {
		short valid_cols[4] = { 2, 1, 3, 0 };

		for (auto& bigram : Bigram.items()) {
			short row0 = this->KeyboardMap[to_keyboard_loc(bigram.key()[0])][0];
			short col0 = this->KeyboardMap[to_keyboard_loc(bigram.key()[0])][1];
			short row1 = this->KeyboardMap[to_keyboard_loc(bigram.key()[1])][0];
			short col1 = this->KeyboardMap[to_keyboard_loc(bigram.key()[1])][1];
			bool is_sfb = false;

			// Sfb: If same col or cols combination is 3,4 or 5,6, exclude same key
			if (col0 == col1 && row0 != row1 || col0 == 3 && col1 == 4
				|| col0 == 4 && col1 == 3 || col0 == 5 && col1 == 6
				|| col0 == 6 && col1 == 5) {
				is_sfb = true;
				this->sfb += (double)bigram.value();
			}

			// Lsb: Middle and Index on the middle column
			if ((col0 / 5 == col1 / 5) && (col0 == 4 || col1 == 4 || col0 == 5 || col1 == 5)
				&& (abs(col0 - col1) >= 2) && (abs(col0 - col1) == 2 || abs(row0 - row1) == 2)) {
				this->lsb += (double)bigram.value();
			}

			// Twist: Longer finger types the lower row, or any uncomfortable Middle-Ring combinations
			//        If same hand, not sfb, and skips home row
			if (col0 / 5 == col1 / 5 && !is_sfb && abs(row0 - row1) == 2) {
				// Mirror the Right col to the Left (5 ~ 9) -> (4 ~ 0)
				if (col0 >= 5) {
					col0 = -col0 + 9;
					col1 = -col1 + 9;
				}
				// Merge 3 and 4 to 3 (0 ~ 3)
				if (col0 == 4) { col0 = 3; }
				if (col1 == 4) { col1 = 3; }

				short temp_list[2] = {};
				short temp_counter = 0;
				short valid_col;
				for (int i = 0; i < 4; i++) {
					valid_col = valid_cols[i];
					if (col0 == valid_col) {
						temp_list[temp_counter] = row0;
						temp_counter++;
						continue;
					}
					if (col1 == valid_col) {
						temp_list[temp_counter] = row1;
						temp_counter++;
					}
				}
				// the longer finger should use the upper row(smaller num) first.
				// Twist: used the longer finger for the lower row(*larger * num)
				// Twist also includes any Middle - Ring combination
				// due to the constraints above, the rows must be different
				bool is_middle_and_ring = col0 == 1 && col1 == 2 || col0 == 2 && col1 == 1;
				if (temp_list[0] > temp_list[1] || is_middle_and_ring) {
					// Twist bigram that involves Pinky is an InTwist
					// for Middle-Ring, InTwist is when Ring uses the upper row (*smaller* num) (like wc on Qwerty)
					if (col0 == 0 || col1 == 0 || is_middle_and_ring && temp_list[0] > temp_list[1]) {
						this->intwist += (double)bigram.value();
					}
					else {
						this->outtwist += (double)bigram.value();
					}
				}
			}
		}
		this->total_twists = this->intwist + this->outtwist;
		this->outtwist_ratio = this->outtwist / this->total_twists;
	}

	/*
	Alternate: pressing one key with one hand, then one with the other, then back to the first
	Roll: pressing two (not same finger) keys with one hand, and a third key with the other
	Onehand: a one-handed trigram where all keys go in the same direction. In other words, a trigram roll.
	Redirect: a one-handed trigram in which the direction changes.
	*/
	void get_trigram_stats() {
		for (auto& trigram : Trigram.items()) {
			short row0 = this->KeyboardMap[to_keyboard_loc(trigram.key()[0])][0];
			short col0 = this->KeyboardMap[to_keyboard_loc(trigram.key()[0])][1];
			short row1 = this->KeyboardMap[to_keyboard_loc(trigram.key()[1])][0];
			short col1 = this->KeyboardMap[to_keyboard_loc(trigram.key()[1])][1];
			short row2 = this->KeyboardMap[to_keyboard_loc(trigram.key()[2])][0];
			short col2 = this->KeyboardMap[to_keyboard_loc(trigram.key()[2])][1];
			short hand0 = col0 / 5;
			short hand1 = col1 / 5;
			short hand2 = col2 / 5;

			// Alternates: Hand A -> Hand B -> Hand A
			if (hand0 != hand1 && hand1 != hand2) {
				this->alternate += (double)trigram.value();
				// an alternate will not be a roll, onehand, or a redirect
				continue;
			}

			// If the same finger is pressed in succession, the trigram is not roll, onehand, or redirect
			if (col0 == col1 || col1 == col2)
				continue;
			// check if (col0, col1) and (col1, col2) in ((3, 4), (4, 3), (5, 6), (6, 5))
			if (abs(col0 - col1) == 1 && (col0 + col1 == 7 || col0 + col1 == 11)
				|| abs(col1 - col2) == 1 && (col1 + col2 == 7 || col1 + col2 == 11))
				continue;

			// onehand and redirect are one-handed trigrams
			if (hand0 == hand1 && hand1 == hand2) {
				// Onehand: all trigram go one direction
				if (col0 > col1 && col1 > col2 || col0 < col1 && col1 < col2)
					this->onehand += (double)trigram.value();
				// Redirect: onehanded trigram without sfb and not Onehand must have changed direction
				else
					this->redirect += (double)trigram.value();
				// onehand or redirect will not be a roll
				continue;
			}

			// Rolls: the rest are rolls
			// Only consider the 2 keys that involves Rolls
			short roll_col0, roll_col1;
			if (hand0 == hand1) {
				roll_col0 = col0;
				roll_col1 = col1;
			}
			else {
				roll_col0 = col1;
				roll_col1 = col2;
			}
			// Mirror the right col to the left
			if (roll_col0 >= 5) {
				roll_col0 = -roll_col0 + 9;
				roll_col1 = -roll_col1 + 9;
			}
			// Inrolls: for cols(0~4), small -> large is rolling inwards
			if (roll_col0 < roll_col1)
				this->inroll += (double)trigram.value();
			// Outrolls: for cols(0~4), large -> small is rolling outwards
			else
				this->outroll += (double)trigram.value();
		}
		this->total_rolls = this->inroll + this->outroll;
		this->inroll_ratio = this->inroll / this->total_rolls;
	}
};

void add_to_arrays(vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
		vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
		vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
		vector<double>& onehand_data, vector<double>& redirect_data, Keyboard& keyboard, int i) {
	effort_data[i] = keyboard.effort;
	hand_diff_data[i] = keyboard.hand_diff;
	sfb_data[i] = keyboard.sfb;
	lsb_data[i] = keyboard.lsb;
	total_twists_data[i] = keyboard.total_twists;
	outtwist_ratio_data[i] = keyboard.outtwist_ratio;
	alternate_data[i] = keyboard.alternate;
	total_rolls_data[i] = keyboard.total_rolls;
	inroll_ratio_data[i] = keyboard.inroll_ratio;
	onehand_data[i] = keyboard.onehand;
	redirect_data[i] = keyboard.redirect;
}

void write_to_file(ofstream& file, string& keyboard_layout, Keyboard& keyboard) {
	file << "Keyboard layout: " << keyboard_layout << '\n' <<
		"Effort: " << keyboard.effort << '\n' <<
		"Hand diff: " << keyboard.hand_diff << '\n' <<
		"Sfb: " << keyboard.sfb << '\n' <<
		"Lsb: " << keyboard.lsb << '\n' <<
		"Total twists: " << keyboard.total_twists << '\n' <<
		"Outtwist ratio: " << keyboard.outtwist_ratio << '\n' <<
		"Alternates: " << keyboard.alternate << '\n' <<
		"Inward roll: " << keyboard.inroll << '\n' <<
		"Outward roll: " << keyboard.outroll << '\n' <<
		"Total rolls: " << keyboard.total_rolls << '\n' <<
		"Inroll ratio: " << keyboard.inroll_ratio << '\n' <<
		"Onehand: " << keyboard.onehand << '\n' <<
		"Redirect: " << keyboard.redirect << endl;
}

bool fullfilled_all_thres(Keyboard& keyboard, vector<double>& thresholds) {
	return (keyboard.effort < thresholds[0] && keyboard.hand_diff < thresholds[1]
			&& keyboard.sfb < thresholds[2] && keyboard.lsb < thresholds[3]
			&& keyboard.total_twists < thresholds[4] && keyboard.outtwist_ratio < thresholds[5]
			&& keyboard.alternate > thresholds[6] && keyboard.total_rolls > thresholds[7]
			&& keyboard.inroll_ratio > thresholds[8] && keyboard.onehand > thresholds[9]
			&& keyboard.redirect < thresholds[10]);
}

void auto_gen(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {

	string remaining_str = Constraints["remaining"];
	string keyboard_str = Constraints["keyboard"];

	// Set random seed to time
	srand(time(0));

	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		// Shuffle list in json, if first item can fit in, add the char to the corresponding position
		for (auto& constraints : Constraints["constraints"].items()) {
			random_shuffle(constraints.value().begin(), constraints.value().end());
			int j;
			for (j = 0; j < constraints.value().size(); j++) {
				if (filled_kb_text[constraints.value()[j]] == '_')
					break;
			}
			char key = constraints.key()[0];
			if (j == constraints.value().size()) {
				cout << key;
				throw runtime_error("Letter busted");
			}
			filled_kb_text[constraints.value()[j]] = constraints.key()[0];
		}

		// Substitute remaining keys
		short remaining_counter = 0;
		random_shuffle(remaining_str.begin(), remaining_str.end());
		for (int j = 0; j < 30; j++) {
			if (filled_kb_text[j] == '_') {
				filled_kb_text[j] = remaining_str[remaining_counter];
				remaining_counter++;
			}
		}

		// Get Keyboard stats
		Keyboard keyboard;
		keyboard.make_keyboard(filled_kb_text);
		keyboard.get_stats();

		// Add stats to the arrays if (is_get_stats)
		if (is_get_stats) {
			add_to_arrays(effort_data, hand_diff_data, sfb_data, lsb_data, total_twists_data, outtwist_ratio_data,
				alternate_data, total_rolls_data, inroll_ratio_data, onehand_data, redirect_data, keyboard, i);
		}
		// Write stats requires the output file, has to fulfill all thresholds
		else {
			if (fullfilled_all_thres(keyboard, thresholds)) { write_to_file(kb_file, filled_kb_text, keyboard); }
		}
	}
}

void auto_get_stats(void func(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds)) {

	int iterations = Constraints["iterations"];
	int stats_iterations = 1'000;  // Sampling size for thresholds
	// Monogram data
	vector<double> effort_data(stats_iterations);
	vector<double> hand_diff_data(stats_iterations);
	// Bigram data
	vector<double> sfb_data(stats_iterations);
	vector<double> lsb_data(stats_iterations);
	vector<double> total_twists_data(stats_iterations);
	vector<double> outtwist_ratio_data(stats_iterations);
	// Trigram data
	vector<double> alternate_data(stats_iterations);
	vector<double> total_rolls_data(stats_iterations);
	vector<double> inroll_ratio_data(stats_iterations);
	vector<double> onehand_data(stats_iterations);
	vector<double> redirect_data(stats_iterations);
	// Output file
	ofstream write_file;
	string write_filepath = "gen" + to_string(Constraints["generation"]) + ".txt";
	write_file.open(write_filepath);
	vector<double> thresholds(11);

	// get the first 1000 stats -> add to arrays
	func(stats_iterations, effort_data, hand_diff_data, sfb_data, lsb_data, total_twists_data, outtwist_ratio_data,
		alternate_data, total_rolls_data, inroll_ratio_data, onehand_data, redirect_data, true, write_file, thresholds);

	std::sort(effort_data.begin(), effort_data.end());
	std::sort(hand_diff_data.begin(), hand_diff_data.end());
	std::sort(sfb_data.begin(), sfb_data.end());
	std::sort(lsb_data.begin(), lsb_data.end());
	std::sort(total_twists_data.begin(), total_twists_data.end());
	std::sort(outtwist_ratio_data.begin(), outtwist_ratio_data.end());
	std::sort(alternate_data.begin(), alternate_data.end());
	std::sort(total_rolls_data.begin(), total_rolls_data.end());
	std::sort(inroll_ratio_data.begin(), inroll_ratio_data.end());
	std::sort(onehand_data.begin(), onehand_data.end());
	std::sort(redirect_data.begin(), redirect_data.end());

	// Set thresholds: get best <cutoffScore>%
	// 1 - <score> means the more the better (inverse)
	thresholds[0] = effort_data[(int)(stats_iterations * (double)CutoffScore["effort"])];
	thresholds[1] = hand_diff_data[(int)(stats_iterations * (double)CutoffScore["hand_diff"])];
	thresholds[2] = sfb_data[(int)(stats_iterations * (double)CutoffScore["sfb"])];
	thresholds[3] = lsb_data[(int)(stats_iterations * (double)CutoffScore["lsb"])];
	thresholds[4] = total_twists_data[(int)(stats_iterations * (double)CutoffScore["total_twists"])];
	thresholds[5] = outtwist_ratio_data[(int)(stats_iterations * (double)CutoffScore["outtwist_ratio"])];
	thresholds[6] = alternate_data[(int)(stats_iterations * (1.0 - (double)CutoffScore["alternate"]))];
	thresholds[7] = total_rolls_data[(int)(stats_iterations * (1.0 - (double)CutoffScore["total_rolls"]))];
	thresholds[8] = inroll_ratio_data[(int)(stats_iterations * (1.0 - (double)CutoffScore["inroll_ratio"]))];
	thresholds[9] = onehand_data[(int)(stats_iterations * (1.0 - (double)CutoffScore["onehand"]))];
	thresholds[10] = redirect_data[(int)(stats_iterations * (double)CutoffScore["redirect"])];
	for (int i = 0; i < 11; i++) {
		cout << thresholds[i] << endl;
	}

	// get the number of iterations -> use thresholds to filter out the results
	func(iterations, effort_data, hand_diff_data, sfb_data, lsb_data, total_twists_data, outtwist_ratio_data,
		alternate_data, total_rolls_data, inroll_ratio_data, onehand_data, redirect_data, false, write_file, thresholds);
}

int main(void) {
	cout << "Generation: " << Constraints["generation"] << endl;
	cout << "Iterations: " << Constraints["iterations"] << endl;
	time_t preTime = time(0) + (time_t)Constraints["iterations"] / 2000;
	int preTime_h = (preTime / 3600) % 24;
	int preTime_m = (preTime / 60) % 60;
	int preTime_s = preTime % 60;
	cout << "Estimated finish time: ";
	printf("UTC %02d:%02d:%02d\n", preTime_h, preTime_m, preTime_s);

	auto startTime = Time::now();
	
	// MAIN FUNCTION
	auto_get_stats(auto_gen);

	auto endTime = Time::now();
	fsec fs = endTime - startTime;
	ms msElapsed = chrono::duration_cast<ms>(fs);
	std::cout << "Time elapsed: " << msElapsed.count() << "ms\n";
	if (msElapsed.count() >= 60000)
		std::cout << "\t(" << msElapsed.count() / 60000.0 << " min)\n";
	std::cin.get();
	return 0;
}