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

ifstream effortTableJson("keyboard_effort_table.json");
json EffortTable = json::parse(effortTableJson);
ifstream monogramJson("n1gram_frequency.json");
json Monogram = json::parse(monogramJson);
ifstream bigramJson("n2gram_frequency.json");
json Bigram = json::parse(bigramJson);
ifstream trigramJson("n3gram_frequency.json");
json Trigram = json::parse(trigramJson);

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
			std::cout << "Expected 30 char for keys, got " << keys.length();
			std::cin.get();
			exit(1);
		}
		this->KeyboardLayout = keys;
		for (int i = 0; i < 30; i++) {
			if (this->KeyboardLayout[i] < 97 || this->KeyboardLayout[i] > 123) {
				continue;
			}
			this->KeyboardMap[KeyboardLayout[i] - 97][0] = i / 10;
			this->KeyboardMap[KeyboardLayout[i] - 97][1] = i % 10;
		}
		for (int i = 26; i < 30; i++) {
			this->KeyboardMap[i][0] = -1;
			this->KeyboardMap[i][1] = -1;
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

	/*
	Gets the total typing effort of the whole keyboard and for individual fingers
	*/
	void get_monogram_stats() {
		double letter_freq;
		double letter_effort;

		for (int i = 0; i < 30; i++) {
			if (this->KeyboardLayout[i] < 97 || this->KeyboardLayout[i] > 123) {
				continue;
			}
			string key = "";
			key += this->KeyboardLayout[i];
			letter_freq = Monogram[key];
			letter_effort = EffortTable["key" + to_string(i)];
			this->effort += letter_freq * letter_effort;
			short col0 = this->KeyboardMap[KeyboardLayout[i] - 97][1];
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
			short row0 = this->KeyboardMap[(int)bigram.key()[0] - 97][0];
			short col0 = this->KeyboardMap[(int)bigram.key()[0] - 97][1];
			short row1 = this->KeyboardMap[(int)bigram.key()[1] - 97][0];
			short col1 = this->KeyboardMap[(int)bigram.key()[1] - 97][1];
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
			short row0 = this->KeyboardMap[trigram.key()[0] - 97][0];
			short col0 = this->KeyboardMap[trigram.key()[0] - 97][1];
			short row1 = this->KeyboardMap[trigram.key()[1] - 97][0];
			short col1 = this->KeyboardMap[trigram.key()[1] - 97][1];
			short row2 = this->KeyboardMap[trigram.key()[2] - 97][0];
			short col2 = this->KeyboardMap[trigram.key()[2] - 97][1];
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

struct kbStats {
	string layout;
	double score;
};

void print_percentile(ofstream& file, vector<double>& sorted_data, int& data_len, string& category) {
	// Write to stats file
	file << category << ':' << endl;
	file << "\tTop 1%: " << sorted_data[(int)(data_len * 0.99)] << endl;
	for (int i = 10; i < 50; i += 10) {
		file << "\tTop " << to_string(i) << "% : " <<
			sorted_data[(int)(data_len * (1 - (double)i / (double)100.0))] << endl;
	}
	file << "\tMedian: " << sorted_data[(int)(data_len * 0.5)] << endl;
	for (int i = 60; i < 100; i += 10) {
		file << "\tBottom " << to_string(100 - i) << "% : " <<
			sorted_data[(int)(data_len * (1 - (double)i / (double)100.0))] << endl;
	}
	file << "\tBottom 1%: " << sorted_data[(int)(data_len * 0.01)] << endl;

	// Print to console, same
	std::cout << category << ':' << endl;
	std::cout << "\tTop 1%: " << sorted_data[(int)(data_len * 0.99)] << endl;
	for (int i = 10; i < 50; i += 10) {
		std::cout << "\tTop " << to_string(i) << "% : " <<
			sorted_data[(int)(data_len * (1 - (double)i / (double)100.0))] << endl;
	}
	std::cout << "\tMedian: " << sorted_data[(int)(data_len * 0.5)] << endl;
	for (int i = 60; i < 100; i += 10) {
		std::cout << "\tBottom " << to_string(100 - i) << "% : " <<
			sorted_data[(int)(data_len * (1 - (double)i / (double)100.0))] << endl;
	}
	std::cout << "\tBottom 1%: " << sorted_data[(int)(data_len * 0.01)] << endl;
}

void get_stats(int iterations, void func(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
		vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
		vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
		vector<double>& onehand_data, vector<double>& redirect_data,
		bool is_get_stats, ofstream& kb_file, vector<double>& thresholds), short gen_num) {
	// Monogram data
	vector<double> effort_data(iterations);
	vector<double> hand_diff_data(iterations);
	// Bigram data
	vector<double> sfb_data(iterations);
	vector<double> lsb_data(iterations);
	vector<double> total_twists_data(iterations);
	vector<double> outtwist_ratio_data(iterations);
	// Trigram data
	vector<double> alternate_data(iterations);
	vector<double> total_rolls_data(iterations);
	vector<double> inroll_ratio_data(iterations);
	vector<double> onehand_data(iterations);
	vector<double> redirect_data(iterations);
	// Output file
	ofstream stats_file;
	string stats_filepath = "gen" + to_string(gen_num) + "_stats.txt";
	stats_file.open(stats_filepath);
	// Not used: Placeholders
	ofstream write_file;
	vector<double> thresholds;

	// call gen{n}()
	func(iterations, effort_data, hand_diff_data, sfb_data, lsb_data, total_twists_data, outtwist_ratio_data,
		alternate_data, total_rolls_data, inroll_ratio_data, onehand_data, redirect_data, true, write_file, thresholds);
	
	sort(effort_data.begin(), effort_data.end());
	sort(hand_diff_data.begin(), hand_diff_data.end());
	sort(sfb_data.begin(), sfb_data.end());
	sort(lsb_data.begin(), lsb_data.end());
	sort(total_twists_data.begin(), total_twists_data.end());
	sort(outtwist_ratio_data.begin(), outtwist_ratio_data.end());
	sort(alternate_data.begin(), alternate_data.end());
	sort(total_rolls_data.begin(), total_rolls_data.end());
	sort(inroll_ratio_data.begin(), inroll_ratio_data.end());
	sort(onehand_data.begin(), onehand_data.end());
	sort(redirect_data.begin(), redirect_data.end());

	// '<' means the less the better, '>' means the more the better
	string effort_text = "Effort < ";
	string hand_diff_text = "HandDiff < ";
	string sfb_text = "Sfb < ";
	string lsb_text = "Lsb < ";
	string total_twists_text = "TotalTwists < ";
	string outtwist_ratio_text = "OuttwistRatio < ";
	string alternate_text = "Alternates > ";
	string total_rolls_text = "TotalRolls > ";
	string inroll_ratio_text = "InrollRatio > ";
	string onehand_text = "Onehand > ";
	string redirect_text = "Redirect < ";

	print_percentile(stats_file, effort_data, iterations, effort_text);
	print_percentile(stats_file, hand_diff_data, iterations, hand_diff_text);
	print_percentile(stats_file, sfb_data, iterations, sfb_text);
	print_percentile(stats_file, lsb_data, iterations, lsb_text);
	print_percentile(stats_file, total_twists_data, iterations, total_twists_text);
	print_percentile(stats_file, outtwist_ratio_data, iterations, outtwist_ratio_text);
	print_percentile(stats_file, alternate_data, iterations, alternate_text);
	print_percentile(stats_file, total_rolls_data, iterations, total_rolls_text);
	print_percentile(stats_file, inroll_ratio_data, iterations, inroll_ratio_text);
	print_percentile(stats_file, onehand_data, iterations, onehand_text);
	print_percentile(stats_file, redirect_data, iterations, redirect_text);
}

void write_stats(int iterations, void func(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds), short gen_num, vector<double>& thresholds) {
	// Output file
	ofstream write_file;
	string write_file_path = "gen" + to_string(gen_num) + ".txt";
	write_file.open(write_file_path);
	// Not used: Placeholders
	vector<double> effort_data, hand_diff_data, sfb_data, lsb_data, total_twists_data, outtwist_ratio_data,
		alternate_data, total_rolls_data, inroll_ratio_data, onehand_data, redirect_data;
	// call gen{n}()
	func(iterations, effort_data, hand_diff_data, sfb_data, lsb_data, total_twists_data, outtwist_ratio_data,
		alternate_data, total_rolls_data, inroll_ratio_data, onehand_data, redirect_data, false, write_file, thresholds);
}

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

void gen1(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
		vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
		vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
		vector<double>& onehand_data, vector<double>& redirect_data,
		bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "abcdfghijklmnopqrstuvwxyz----";
	string keyboard_str = "______________________________";
	short e_loc[4] = { 16, 17, 18, 19 };

	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(e_loc, e_loc + 4); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';

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

void gen2(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bcdfgjklmpqsuvwxyz----";
	string keyboard_str = "______________________________";
	short e_loc[4] = { 16, 17, 18, 19 };
	short a_loc[4] = { 10, 12, 17, 18 };
	short i_loc[5] = { 11, 12, 16, 17, 18 };
	string ot_str = "ot";
	short ot_loc[10] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
	string nr_str = "nr";
	short nr_loc[12] = { 0, 1, 2, 3, 10, 11, 12, 13, 20, 21, 22, 23 };
	short h_loc[12] = { 0, 2, 3, 9, 10, 12, 13, 19, 20, 22, 23, 29 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(e_loc, e_loc + 4); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';
		do { random_shuffle(a_loc, a_loc + 4); } while (filled_kb_text[a_loc[0]] != '_');
		filled_kb_text[a_loc[0]] = 'a';
		do { random_shuffle(i_loc, i_loc + 5); } while (filled_kb_text[i_loc[0]] != '_');
		filled_kb_text[i_loc[0]] = 'i';
		for (int j = 0; j < ot_str.length(); j++) {
			do { random_shuffle(ot_loc, ot_loc + 10); } while (filled_kb_text[ot_loc[0]] != '_');
			filled_kb_text[ot_loc[0]] = ot_str[j];
		}
		for (int j = 0; j < nr_str.length(); j++) {
			do { random_shuffle(nr_loc, nr_loc + 10); } while (filled_kb_text[nr_loc[0]] != '_');
			filled_kb_text[nr_loc[0]] = nr_str[j];
		}
		do { random_shuffle(h_loc, h_loc + 12); } while (filled_kb_text[h_loc[0]] != '_');
		filled_kb_text[h_loc[0]] = 'h';

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

void gen3(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bcdfgjkpqsvwz----";
	string keyboard_str = "______________________________";
	short e_loc[4] = { 16, 17, 18, 19 };
	string aoitn_str = "aoitn";
	short aoitn_loc[8] = { 10, 11, 12, 13, 16, 17, 18, 19 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short h_loc[12] = { 0, 2, 3, 9, 10, 12, 13, 19, 20, 22, 23, 29 };
	short r_loc[12] = { 0, 1, 2, 3, 10, 11, 12, 13, 20, 21, 22, 23 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short y_loc[12] = { 5, 6, 7, 8, 15, 16, 17, 18, 25, 26, 27, 28 };
	short u_loc[12] = { 5, 7, 8, 9, 15, 17, 18, 19, 25, 27, 28, 29 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(e_loc, e_loc + 4); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';
		for (int j = 0; j < aoitn_str.length(); j++) {
			do { random_shuffle(aoitn_loc, aoitn_loc + 8); } while (filled_kb_text[aoitn_loc[0]] != '_');
			filled_kb_text[aoitn_loc[0]] = aoitn_str[j];
		}
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(h_loc, h_loc + 12); } while (filled_kb_text[h_loc[0]] != '_');
		filled_kb_text[h_loc[0]] = 'h';
		do { random_shuffle(r_loc, r_loc + 12); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(y_loc, y_loc + 12); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';
		do { random_shuffle(u_loc, u_loc + 12); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';

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

void gen4(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bcdfgjkpqsvwz----";
	string keyboard_str = "______________________________";
	short e_loc[3] = { 16, 17, 18 };
	short o_loc[3] = { 11, 12, 17 };
	short i_loc[4] = { 16, 17, 18, 19 };
	short n_loc[4] = { 10, 12, 13, 17 };
	short a_loc[5] = { 11, 16, 17, 18, 19 };
	short t_loc[5] = { 10, 11, 12, 13, 19 };
	short h_loc[9] = { 2, 3, 9, 12, 13, 19, 22, 23, 29 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };
	short r_loc[12] = { 0, 1, 2, 3, 10, 11, 12, 13, 20, 21, 22, 23 };
	short u_loc[12] = { 5, 7, 8, 9, 15, 17, 18, 19, 25, 27, 28, 29 };
	short y_loc[12] = { 5, 6, 7, 8, 15, 16, 17, 18, 25, 26, 27, 28 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(e_loc, e_loc + 3); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';
		do { random_shuffle(o_loc, o_loc + 3); } while (filled_kb_text[o_loc[0]] != '_');
		filled_kb_text[o_loc[0]] = 'o';
		do { random_shuffle(i_loc, i_loc + 4); } while (filled_kb_text[i_loc[0]] != '_');
		filled_kb_text[i_loc[0]] = 'i';
		do { random_shuffle(n_loc, n_loc + 4); } while (filled_kb_text[n_loc[0]] != '_');
		filled_kb_text[n_loc[0]] = 'n';
		do { random_shuffle(a_loc, a_loc + 5); } while (filled_kb_text[a_loc[0]] != '_');
		filled_kb_text[a_loc[0]] = 'a';
		do { random_shuffle(t_loc, t_loc + 5); } while (filled_kb_text[t_loc[0]] != '_');
		filled_kb_text[t_loc[0]] = 't';
		do { random_shuffle(h_loc, h_loc + 9); } while (filled_kb_text[h_loc[0]] != '_');
		filled_kb_text[h_loc[0]] = 'h';
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';
		do { random_shuffle(r_loc, r_loc + 12); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(u_loc, u_loc + 12); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(y_loc, y_loc + 12); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';

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

void gen5(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bcdfgjkpqwz----";
	string keyboard_str = "______________________________";
	short a_loc[3] = { 16, 17, 18 };
	short e_loc[3] = { 16, 17, 18 };
	short n_loc[3] = { 10, 12, 13 };
	short i_loc[4] = { 16, 17, 18, 19 };
	short t_loc[4] = { 10, 11, 12, 13 };
	short h_loc[6] = { 3, 9, 13, 19, 23, 29 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short o_loc[12] = { 2, 3, 7, 8, 10, 11, 12, 13, 16, 17, 18, 19 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };
	short r_loc[12] = { 0, 1, 2, 3, 10, 11, 12, 13, 20, 21, 22, 23 };
	short u_loc[12] = { 5, 7, 8, 9, 15, 17, 18, 19, 25, 27, 28, 29 };
	short y_loc[12] = { 5, 6, 7, 8, 15, 16, 17, 18, 25, 26, 27, 28 };
	short s_loc[15] = { 0, 1, 2, 3, 6, 10, 11, 12, 13, 16, 20, 21, 22, 23, 26 };
	short v_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(a_loc, a_loc + 3); } while (filled_kb_text[a_loc[0]] != '_');
		filled_kb_text[a_loc[0]] = 'a';
		do { random_shuffle(e_loc, e_loc + 3); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';
		do { random_shuffle(n_loc, n_loc + 3); } while (filled_kb_text[n_loc[0]] != '_');
		filled_kb_text[n_loc[0]] = 'n';
		do { random_shuffle(i_loc, i_loc + 4); } while (filled_kb_text[i_loc[0]] != '_');
		filled_kb_text[i_loc[0]] = 'i';
		do { random_shuffle(t_loc, t_loc + 4); } while (filled_kb_text[t_loc[0]] != '_');
		filled_kb_text[t_loc[0]] = 't';
		do { random_shuffle(h_loc, h_loc + 6); } while (filled_kb_text[h_loc[0]] != '_');
		filled_kb_text[h_loc[0]] = 'h';
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(o_loc, o_loc + 12); } while (filled_kb_text[o_loc[0]] != '_');
		filled_kb_text[o_loc[0]] = 'o';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';
		do { random_shuffle(r_loc, r_loc + 12); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(u_loc, u_loc + 12); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(y_loc, y_loc + 12); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';
		do { random_shuffle(s_loc, s_loc + 15); } while (filled_kb_text[s_loc[0]] != '_');
		filled_kb_text[s_loc[0]] = 's';
		do { random_shuffle(v_loc, v_loc + 15); } while (filled_kb_text[v_loc[0]] != '_');
		filled_kb_text[v_loc[0]] = 'v';

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

void gen6(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bcdfgjkpqwz----";
	string keyboard_str = "______________________________";
	
	short o_loc[2] = { 7, 8 };
	short t_loc[3] = { 11, 12, 13 };
	short a_loc[3] = { 16, 17, 18 };
	short e_loc[3] = { 16, 17, 18 };
	short n_loc[3] = { 10, 12, 13 };
	short i_loc[4] = { 16, 17, 18, 19 };
	short s_loc[4] = { 10, 11, 12, 13 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short h_loc[13] = { 0, 1, 2, 3, 10, 11, 12, 13, 19, 20, 21, 22, 23 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };
	short r_loc[12] = { 0, 1, 2, 3, 10, 11, 12, 13, 20, 21, 22, 23 };
	short u_loc[12] = { 5, 7, 8, 9, 15, 17, 18, 19, 25, 27, 28, 29 };
	short y_loc[12] = { 5, 6, 7, 8, 15, 16, 17, 18, 25, 26, 27, 28 };
	short v_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(o_loc, o_loc + 2); } while (filled_kb_text[o_loc[0]] != '_');
		filled_kb_text[o_loc[0]] = 'o';
		do { random_shuffle(t_loc, t_loc + 3); } while (filled_kb_text[t_loc[0]] != '_');
		filled_kb_text[t_loc[0]] = 't';
		do { random_shuffle(a_loc, a_loc + 3); } while (filled_kb_text[a_loc[0]] != '_');
		filled_kb_text[a_loc[0]] = 'a';
		do { random_shuffle(e_loc, e_loc + 3); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';
		do { random_shuffle(n_loc, n_loc + 3); } while (filled_kb_text[n_loc[0]] != '_');
		filled_kb_text[n_loc[0]] = 'n';
		do { random_shuffle(i_loc, i_loc + 4); } while (filled_kb_text[i_loc[0]] != '_');
		filled_kb_text[i_loc[0]] = 'i';
		do { random_shuffle(s_loc, s_loc + 4); } while (filled_kb_text[s_loc[0]] != '_');
		filled_kb_text[s_loc[0]] = 's';
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(h_loc, h_loc + 13); } while (filled_kb_text[h_loc[0]] != '_');
		filled_kb_text[h_loc[0]] = 'h';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';
		do { random_shuffle(r_loc, r_loc + 12); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(u_loc, u_loc + 12); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(y_loc, y_loc + 12); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';
		do { random_shuffle(v_loc, v_loc + 15); } while (filled_kb_text[v_loc[0]] != '_');
		filled_kb_text[v_loc[0]] = 'v';


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

void gen7(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bdfgjkpqwz----";
	string keyboard_str = "___________________h__________";

	short a_loc[2] = { 17, 18 };
	short e_loc[2] = { 17, 18 };
	short o_loc[2] = { 7, 8 };
	short t_loc[3] = { 11, 12, 13 };
	short i_loc[3] = { 16, 17, 18 };
	short n_loc[4] = { 10, 11, 12, 13 };
	short s_loc[4] = { 10, 11, 12, 13 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };
	short r_loc[12] = { 0, 1, 2, 3, 10, 11, 12, 13, 20, 21, 22, 23 };
	short u_loc[12] = { 5, 6, 7, 8, 15, 16, 17, 18, 25, 26, 27, 28 };
	short y_loc[12] = { 5, 6, 7, 8, 15, 16, 17, 18, 25, 26, 27, 28 };
	short c_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };
	short v_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(a_loc, a_loc + 2); } while (filled_kb_text[a_loc[0]] != '_');
		filled_kb_text[a_loc[0]] = 'a';
		do { random_shuffle(e_loc, e_loc + 2); } while (filled_kb_text[e_loc[0]] != '_');
		filled_kb_text[e_loc[0]] = 'e';
		do { random_shuffle(o_loc, o_loc + 2); } while (filled_kb_text[o_loc[0]] != '_');
		filled_kb_text[o_loc[0]] = 'o';
		do { random_shuffle(i_loc, i_loc + 3); } while (filled_kb_text[i_loc[0]] != '_');
		filled_kb_text[i_loc[0]] = 'i';
		do { random_shuffle(t_loc, t_loc + 3); } while (filled_kb_text[t_loc[0]] != '_');
		filled_kb_text[t_loc[0]] = 't';
		do { random_shuffle(n_loc, n_loc + 4); } while (filled_kb_text[n_loc[0]] != '_');
		filled_kb_text[n_loc[0]] = 'n';
		do { random_shuffle(s_loc, s_loc + 4); } while (filled_kb_text[s_loc[0]] != '_');
		filled_kb_text[s_loc[0]] = 's';
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';
		do { random_shuffle(r_loc, r_loc + 12); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(u_loc, u_loc + 12); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(y_loc, y_loc + 12); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';
		do { random_shuffle(c_loc, c_loc + 15); } while (filled_kb_text[c_loc[0]] != '_');
		filled_kb_text[c_loc[0]] = 'c';
		do { random_shuffle(v_loc, v_loc + 15); } while (filled_kb_text[v_loc[0]] != '_');
		filled_kb_text[v_loc[0]] = 'v';


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

void gen8(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bfgjkpqwz----";
	string keyboard_str = "________________iaeh__________";

	short o_loc[2] = { 7, 8 };
	short n_loc[3] = { 10, 11, 12 };
	short t_loc[3] = { 11, 12, 13 };
	short r_loc[4] = { 10, 11, 12, 13 };
	short s_loc[4] = { 10, 11, 12, 13 };
	short u_loc[4] = { 5, 6, 7, 8 };
	short y_loc[5] = { 5, 6, 15, 25, 26 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };
	short d_loc[15] = { 1, 2, 3, 4, 9, 11, 12, 13, 14, 21, 22, 23, 24, 29 };
	short v_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };
	short c_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(o_loc, o_loc + 2); } while (filled_kb_text[o_loc[0]] != '_');
		filled_kb_text[o_loc[0]] = 'o';
		do { random_shuffle(n_loc, n_loc + 3); } while (filled_kb_text[n_loc[0]] != '_');
		filled_kb_text[n_loc[0]] = 'n';
		do { random_shuffle(t_loc, t_loc + 3); } while (filled_kb_text[t_loc[0]] != '_');
		filled_kb_text[t_loc[0]] = 't';
		do { random_shuffle(r_loc, r_loc + 4); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(s_loc, s_loc + 4); } while (filled_kb_text[s_loc[0]] != '_');
		filled_kb_text[s_loc[0]] = 's';
		do { random_shuffle(u_loc, u_loc + 4); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(y_loc, y_loc + 5); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';
		do { random_shuffle(d_loc, d_loc + 15); } while (filled_kb_text[d_loc[0]] != '_');
		filled_kb_text[d_loc[0]] = 'd';
		do { random_shuffle(v_loc, v_loc + 15); } while (filled_kb_text[v_loc[0]] != '_');
		filled_kb_text[v_loc[0]] = 'v';
		do { random_shuffle(c_loc, c_loc + 15); } while (filled_kb_text[c_loc[0]] != '_');
		filled_kb_text[c_loc[0]] = 'c';


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

void gen9(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bfgjkpqz----";
	string keyboard_str = "_______o________iaeh__________";

	short u_loc[2] = { 6, 8 };
	short y_loc[2] = { 25, 26 };
	short n_loc[3] = { 10, 11, 12 };
	short t_loc[3] = { 11, 12, 13 };
	short r_loc[4] = { 10, 11, 12, 13 };
	short s_loc[4] = { 10, 11, 12, 13 };
	short x_loc[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	short l_loc[12] = { 0, 2, 3, 4, 10, 12, 13, 14, 20, 22, 23, 24 };
	short m_loc[12] = { 0, 1, 3, 4, 10, 11, 13, 14, 20, 21, 23, 24 };
	short d_loc[14] = { 1, 2, 3, 4, 9, 11, 12, 13, 14, 21, 22, 23, 24, 29 };
	string cwv_str = "cwv";
	short cwv_loc[15] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24 };


	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

		do { random_shuffle(u_loc, u_loc + 2); } while (filled_kb_text[u_loc[0]] != '_');
		filled_kb_text[u_loc[0]] = 'u';
		do { random_shuffle(y_loc, y_loc + 2); } while (filled_kb_text[y_loc[0]] != '_');
		filled_kb_text[y_loc[0]] = 'y';
		do { random_shuffle(n_loc, n_loc + 3); } while (filled_kb_text[n_loc[0]] != '_');
		filled_kb_text[n_loc[0]] = 'n';
		do { random_shuffle(t_loc, t_loc + 3); } while (filled_kb_text[t_loc[0]] != '_');
		filled_kb_text[t_loc[0]] = 't';
		do { random_shuffle(r_loc, r_loc + 4); } while (filled_kb_text[r_loc[0]] != '_');
		filled_kb_text[r_loc[0]] = 'r';
		do { random_shuffle(s_loc, s_loc + 4); } while (filled_kb_text[s_loc[0]] != '_');
		filled_kb_text[s_loc[0]] = 's';
		do { random_shuffle(x_loc, x_loc + 10); } while (filled_kb_text[x_loc[0]] != '_');
		filled_kb_text[x_loc[0]] = 'x';
		do { random_shuffle(l_loc, l_loc + 12); } while (filled_kb_text[l_loc[0]] != '_');
		filled_kb_text[l_loc[0]] = 'l';
		do { random_shuffle(m_loc, m_loc + 12); } while (filled_kb_text[m_loc[0]] != '_');
		filled_kb_text[m_loc[0]] = 'm';
		do { random_shuffle(d_loc, d_loc + 14); } while (filled_kb_text[d_loc[0]] != '_');
		filled_kb_text[d_loc[0]] = 'd';
		for (int j = 0; j < cwv_str.length(); j++) {
			do { random_shuffle(cwv_loc, cwv_loc + 15); } while (filled_kb_text[cwv_loc[0]] != '_');
			filled_kb_text[cwv_loc[0]] = cwv_str[j];
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

void gen10(int& iterations, vector<double>& effort_data, vector<double>& hand_diff_data, vector<double>& sfb_data,
	vector<double>& lsb_data, vector<double>& total_twists_data, vector<double>& outtwist_ratio_data,
	vector<double>& alternate_data, vector<double>& total_rolls_data, vector<double>& inroll_ratio_data,
	vector<double>& onehand_data, vector<double>& redirect_data,
	bool is_get_stats, ofstream& kb_file, vector<double>& thresholds) {
	string remaining_str = "bjkwvxz----";
	string keyboard_str = "_lcm__qoupnrstg_iaeh__fd__y___";

	// Set random seed to time
	srand(time(0));
	for (int i = 0; i < iterations; i++) {
		string filled_kb_text = keyboard_str;

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


int main(void) {
	typedef chrono::high_resolution_clock Time;
	typedef chrono::milliseconds ms;
	typedef chrono::duration<float> fsec;
	auto startTime = Time::now();

	int stats_iterations = 1'000;
	int write_iterations = 2'000'000;
	// Generation 1
	// get_stats(stats_iterations, gen1, 1);
	vector<double> thres1 = { 89.4094, 10.1845, 6.71806, 4.1403, 3.04481, 0.500164,
							30.0939, 38.5164, 0.512739, 2.70094, 9.14206 };
	// write_stats(write_iterations, gen1, 1, thres1);

	// Generation 2
	// get_stats(stats_iterations, gen2, 2);
	vector<double> thres2 = { 83.2821, 7.1407, 6.27343, 3.22599, 1.02268, 0.507222,
							32.7325, 39.3461, 0.505499, 2.86618, 9.04645 };
	// write_stats(write_iterations, gen2, 2, thres2);

	// Generation 3
	// get_stats(stats_iterations, gen3, 3);
	vector<double> thres3 = { 79.6144, 6.13933, 5.979, 2.53191, 0.75723, 0.458516,
							34.2729, 39.0725, 0.513574, 2.80375, 9.44893 };
	// write_stats(write_iterations, gen3, 3, thres3);

	// Generation 4
	// get_stats(stats_iterations, gen4, 4);
	vector<double> thres4 = { 78.0474, 7.59425, 5.38955, 2.53191, 0.806351, 0.462645,
							36.159, 39.3035, 0.537363, 2.68778, 7.54643 };
	// write_stats(write_iterations, gen4, 4, thres4);

	// Generation 5
	// get_stats(stats_iterations, gen5, 5);
	vector<double> thres5 = { 79.2437, 7.59425, 4.25382, 2.92565, 0.806351, 0.515759,
							39.3995, 39.4583, 0.549892, 1.55073, 5.32996 };
	// write_stats(write_iterations, gen5, 5, thres5);

	// Generation 6
	// get_stats(stats_iterations, gen6, 6);
	vector<double> thres6 = { 77.8098, 5.70048, 3.86215, 2.68663, 0.864295, 0.484539,
							42.3241, 39.7835, 0.570599, 1.20804, 4.18111 };
	// write_stats(write_iterations, gen6, 6, thres6);

	// Generation 7
	// get_stats(stats_iterations, gen7, 7);
	vector<double> thres7 = { 76.3865, 4.63001, 2.42603, 2.02903, 0.864295, 0.463861,
							42.0869, 40.0864, 0.60725, 1.20331, 3.67339 };
	// write_stats(write_iterations, gen7, 7, thres7);

	// Generation 8
	// get_stats(stats_iterations, gen8, 8);
	vector<double> thres8 = { 76.3865, 4.63001, 1.80000, 2.02903, 0.864295, 0.463861,
							42.0869, 40.0864, 0.60725, 1.20331, 3.67339 };
	// write_stats(write_iterations, gen8, 8, thres8);

	// Generation 9
	// get_stats(stats_iterations, gen9, 9);
	vector<double> thres9 = { 76.1465, 4.63001, 1.40000, 1.72081, 0.864295, 0.463861,
							42.0869, 41.7296, 0.630459, 1.20331, 3.67339 };
	// write_stats(write_iterations, gen9, 9, thres9);

	// Generation 10
	// get_stats(stats_iterations, gen10, 10);
	vector<double> thres10 = { 76.1465, 4.63001, 1.00000, 1.72081, 0.864295, 0.463861,
							42.0869, 41.7296, 0.630459, 1.20331, 3.67339 };
	write_stats(write_iterations, gen10, 10, thres10);


	auto endTime = Time::now();
	fsec fs = endTime - startTime;
	ms msElapsed = chrono::duration_cast<ms>(fs);
	std::cout << "Time elapsed: " << msElapsed.count() << "ms\n";
	if (msElapsed.count() >= 60000)
		std::cout << "\t(" << msElapsed.count() / 60000.0 << " min)\n";
	std::cin.get();
	return 0;
}