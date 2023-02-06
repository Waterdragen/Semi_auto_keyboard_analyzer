# Semi-manual Keyboard Analyzer
this project is only in Stage 1 (you have to edit and re-run the source code for each and every generation)
this analyzer ignores punctuation n-grams. The empty keys are represented as '-'

# Documentations (analyzer.cpp)

### Reminder: You only need to understand `get_stats()`, `write_stats()`, `gen1()` and `main()` to make the program work. You can skip this part.
Added as proof of progress.

### Keyboard (class object)
stores a keyboard with a given layout string. The class methods initialize the keyboard and compute the statistics of the keyboard.

### Keyboard.class_variables

| Variable Name | Variable Type | Description |
| ------------- | ------------- | ----------- |
| `Keyboard.KeyboardLayout` | `string` (`.length() == 30`) | String representation of the `Keyboard`. For example: `"qwfpbjluy;arstgmneiozxcdvkh,./"` represents a Colemak-DH keyboard, where the keys start from left-to-right, top-to-bottom order |
| `Keyboard.KeyboardMap` | `short[30][2]` | An array that stores the locations of the keys of the `Keyboard`. The first 26 items store the (row, column) of the corresponding letter where `'a' == 0`, ... , `'z' == 25`. The last 4 items are placeholders and they are not used. |
| `Keyboard.effort` | `double` | The typing effort of the `Keyboard`. It is calculated based on Colemak-DH's typing effort model. However, the typing effort model is scaled down so that the average typing effort is `1.0`. This is done so that a randomly generated Keyboard will have a typing effort of `100.0` |
| `Keyboard.hand_diff` | `double` | The typing effort difference of the Left Hand and the Right Hand. |
| `Keyboard.sfb` | `double` | Stands for Same Finger Bigram. Bigrams typed using the same Fingers. |
| `Keyboard.lsb` | `double` | Stands for Lateral Stretch Bigram. Bigrams that require lateral stretching (any of column combinations 2, 4 or 5, 7). |
| `Keyboard.intwist` | `double` | Commonly known as Scissors in AKL. Bigrams in which the wrist twists inwards. |
| `Keyboard.outtwist` | `double` | Commonly known as Scissors in AKL. Bigrams in which the wrist twists outwards. |
| `Keyboard.total_twists` | `double` | The sum of InTwists and OutTwists. |
| `Keyboard.outtwist_ratio` | `double` | The ratio of OutTwists to Total Twists. |
| `Keyboard.alternate` | `double` | Trigrams typed with one hand, then one with the other, then back to the first. |
| `Keyboard.inroll` | `double` | Trigrams where two (not same finger) keys are typed with one hand, and a third key with the other, and goes from outer columns to inner columns. |
| `Keyboard.outroll` | `double` | Trigrams where two (not same finger) keys are typed with one hand, and a third key with the other, and goes from inner columns to outer columns. |
| `Keyboard.total_rolls` | `double` | The sum of InRolls and OutRolls. |
| `Keyboard.inroll_ratio` | `double` | The ratio of InRolls to Total Rolls. |
| `Keyboard.onehand` | `double` | One-handed Trigrams where all keys go in the same direction. |
| `Keyboard.redirect` | `double` | One-handed Trigram in which the direction changes. |

### Keyboard.class_methods()

| Method Name | Parameter(s) Type | Description |
| ----------- | ----------------- | ----------- |
| `Keyboard.make_keyboard()` | `string` (`.length == 30`) | Initializes the `Keyboard` with the given `string`. |
| `Keyboard.pretty_print()` | `void` | Pretty prints the stats to the console. |
| `Keyboard.get_stats()` | `void` | Gets Monogram stats, Bigram stats, and Trigram stats. |
| `Keyboard.write_stats()` | `ofstream` | Writes the stats to `ofstream`. |
| `Keyboard.get_monogram_stats()` | `void` | Calculates the stats of `effort` and `hand_diff`. |
| `Keyboard.get_bigram_stats()` | `void` | Calculates the stats of `sfb`, `lsb`, `intwist`, `outtwist`, `total_twists`, and `outtwist_ratio`. |
| `Keyboard.get_trigram_stats()` | `void` | Calculates the stats of `alternate`, `inroll`, `outroll`, `total_rolls`, `inroll_ratio`, `onehand`, and `redirect`. |

### Intermediate functions
| Function Name | Parameter(s) Type | Description |
| ------------- | ------------- | ----------- |
| `print_percentile()` | `ofstream`, `vector<double>`, `int`, `string` | Gets the percentile of the stats. Writes them to a text file and prints them to the console. |
| `add_to_arrays()` | `vector<double>`, ... , `Keyboard`, `int` | Adds the `Keyboard.class_variables` to the corresponding `vector<double>` |
| `write_to_file()` | `ofstream`, `string`, `Keyboard` | Writes the `Keyboard` stats to a text file. |
| `fullfilled_all_thres()` | `Keyboard`, `vector<double>` | Check if the `Keyboard` has fulfilled all the thresholds.  |

## Main functions
| Function Name | Parameter(s) Type | Description |
| ------------- | ----------------- | ----------- |
| **`get_stats()`** | iterations: `int`, func: `void(...)` (should be similar to gen1), gen_num: `int` | Gets (a small number, like `1000`) `Keyboard` stats samples using the random `Keyboard` generator like `gen1()`. Writes the percentiles to a text file and prints to the console. You can use the percentiles to set the thresholds. The generation number should match the number of the function, to rename the file using that number. |
| **`write_stats()`** | iterations: `int`, func: `void()` (should be similar to gen1), gen_num: `int`, thresholds: `vector<double` | Gets (a large number, like `5'000'000`) `Keyboard` stats using the random `Keyboard` generator like `gen1()`. Writes the stats that fulfilled all the thresholds to a text file. It takes around 4.2 minutes to compute `1'000'000` keyboard layouts. |
| **`gen1()`** | iterations: `int`, `vector<double>`, ... , is_get_stats: `bool`, kb_file: `ofstream`, thresholds: `vector<double>` | You may need to edit and/or copy this function but you don't have to change the parameters. Randomly generates `Keyboard` for <`iterations`> times. See below for more details. |

| **`main()`** | `void` | You may need to edit this function. get the percentile of the stats / write the stats to a text file. See below for more details. |

### gen1() (or other names)

`remaining_str` stores the letters that are not constrained (row or col) for the keyboard.  <br>
Example: `string remaining_str = "abcdfghijklmnopqrstuvwxyz----";` (notice the e is missing because it's constrained)   <br>

`keyboard_str` stores the letters that are locked in place. Vacant spots are represented as `'_'`.  <br>
Example: `string keyboard_str = "______________________________";`  <br>

`e_loc` (or equivalent) stores the possible locations of the keyboard (constraints).  <br>
Example: `e_loc[4] = { 16, 17, 18, 19 };  <br>
This means e has 4 possible locations: 16, 17, 18, or 19.  <br>

You can make multiple constraints but make sure that none of them is *busted*  <br>
Bad example:  <br>
```
a_loc[2] = [17, 18]
e_loc[2] = [17, 18]
o_loc[2] = [17, 18]
```
The a and e have to fill 17 and 18, leaving o nowhere to place them and you'll get stuck in a loop.  <br>

make sure you do not remove this line: `string filled_kb_text = keyboard_str;`  <br>
this makes a copy of the original string `keyboard_str`  <br>

```
do { random_shuffle(e_loc, e_loc + 4); } while (filled_kb_text[e_loc[0]] != '_');
filled_kb_text[e_loc[0]] = 'e';
```
This means shuffle e's 4 possible positions until a vacant space is found. Make `_` -> `e`.
Copy these two lines to constrain multiple letters.

Do not change the remaining code.

### main()
`stats_iterations`: Sampling size for getting the percentile stats. No need to be huge.
`int stats_iterations = 1'000;` recommended
`int stats_iterations = 10'000'000;` *could've used that time on `write_stats()`?*
`write_iterations`: Full sampling size that checks against thresholds and writes to text file.

```
	// Generation 1
	// get_stats(stats_iterations, gen1, 1);
	vector<double> thres1 = { 89.4094, 10.1845, 6.71806, 4.1403, 3.04481, 0.500164,
							30.0939, 38.5164, 0.512739, 2.70094, 9.14206 };
	// write_stats(write_iterations, gen1, 1, thres1);
```
Either get the stats percentile of gen1, or write the stats to the text file, depending on the line you uncomment.
Copy this section to advance to the next generation.

The console will print the time elapsed when the program is done.

# Documentations (hash_table_writer.py)

You only need to edit statements inside `if __name__ == '__main__':` to make the program work.

### Functions

| Function Name | Parameter(s) Type | Description |
| ------------- | ----------------- | ----------- |
| `keyboard_effort_table()`| `None` | Write a .json file of a effort table based on Colemak-DH's effort model. |
| `make_hash_tables()` | generation number: `int` | Writes 3 .json files: Row occurrence counter hash table, Row occurrence difference counter hash table, Letter's column occurrence hash table |
| `best_keyboard()` | generation number: `int` | Prints the best keyboards of the generation to the console. |
| `row_col_intersection()` | GenRow: `dict[str, int]`, GenCol: `dict[str, Union[int, tuple[int, ...]]]` | Finds and prints all possible key positions constraints based on the given row and columns. There can only be 1 fixed row but can have multiple columns. |


### Tips

You don't have to always lock a key in place. Sometimes I will free some keys' constraints if the keys seem to get stuck in some weird locations. It took me 7 generations to finally lock h in 19. (No *tightly* fixed keys previously) Turns out 16 was better to balance the pinky effort.

### Results

Here are my results after the 10th generation:
```
  ; l f m k  ' q o u p  
  n r s t g  , i a e h /
  x w c d v  j y z . b  
```
The refined version (Colevrak):
```
  b l f m ;  ' p u o q  
  n r s t g  , h e a i /
  x w c d v  k y z . j  
```
Stats (MT-QUOTES):
  Alt: 31.61%
  Rol: 42.42%   (In/Out: 21.54% | 20.88%)
  One:  1.03%   (In/Out:  0.47% |  0.56%)
  Red:  3.44%   (Bad:     0.12%)
  SFB: 1.08%
  SFS: 6.12%
  LH/RH: 48.52% | 51.48%
  
It's uploaded to AKL discord. You can view the stats using `!amini view colevrak` to view my keyboard layout
