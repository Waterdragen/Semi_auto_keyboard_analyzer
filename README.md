# Semi-manual Keyboard Analyzer
this project is only in Stage 1 (you have to edit and re-run the source code for each and every generation)
this analyzer ignores punctuation n-grams. The empty keys are represented as '-'

# Documentations (analyzer.cpp)

### Keyboard (class object)
stores a keyboard with given layout string. The class methods initialize the keyboard and computes the statistics of the keyboard.

### Keyboard.class_variables

| Variable Name | Variable Type | Description |
| ------------- | ------------- | ----------- |
| `Keyboard.KeyboardLayout` | `string` (`.length() == 30`) | String representation of the `Keyboard`. For example: `"qwfpbjluy;arstgmneiozxcdvkh,./"` represents a Colemak-DH keyboard, where the keys start from left-to-right, top-to-bottom order |
| `Keyboard.KeyboardMap` | `short[30][2]` | An array which stores the locations of the keys of the `Keyboard`. The first 26 items store the (row, column) of the corresponding letter where `'a' == 0`, ... , `'z' == 26`. The last 4 itmes are placeholders and they are not used. |
| `Keyboard.effort` | `double` | The typing effort of the Keyboard. It is calculated based on Colemak-DH's typing effort model. However, the typing effort model is scaled down so that the average typing effort is `1.0`. This is done so that a randomly generated Keyboard will have a typing effort of `100.0` |
| `Keyboard.hand_diff` | `double` | The typing effort difference of the Left Hand and the Right Hand. |
| `Keyboard.sfb` | `double` | Stands for Same Finger Bigram. Bigrams typed using the same Fingers. |
| `Keyboard.lsb` | `double` | Stands for Lateral Stretch Bigram. Bigrams that require lateral stretching (any of column combinations 2, 4 or 5, 7). |
| `Keyboard.intwist` | `double` |  |
| `Keyboard.outtwist` | `double` |  |
| `Keyboard.total_twists` | `double` | The sum of InTwists and OutTwists. |
| `Keyboard.outtwist_ratio` | `double` | The ratio of OutTwists to Total Twists. |
| `Keyboard.alternate` | `double` | Trigrams typed with one hand, then one with the other, then back to the first. |
| `Keyboard.inroll` | `double` | Trigrams where two (not same finger) keys typed with one hand, and a third key with the other, and goes from outer columns to inner columns. |
| `Keyboard.outroll` | `double` | Trigrams where two (not same finger) keys typed with one hand, and a third key with the other, and goes from inner columns to outer columns. |
| `Keyboard.total_rolls` | `double` | The sum of InRolls and OutRolls. |
| `Keyboard.inroll_ratio` | `double` | The ratio of InRolls to Total Rolls. |
| `Keyboard.onehand` | `double` | One-handed Trigrams where all keys go in the same direction. |
| `Keyboard.redirect` | `double` | One-handed Trigram in which the direction changes. |

| Method Name | Parameters Type | Description |
| ------------- | ------------- | ----------- |
| `` | `` |  |

### Keyboard.class_methods()

# Documentations (analyzer.py)

### Functions
