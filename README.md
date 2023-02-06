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
| `Keyboard.KeyboardLocation` | `short[30][2]` | An array which stores the locations of the keys of the `Keyboard`. The first 26 items store the (row, column) of the corresponding letter where `'a' == 0`, ... , `'z' == 26`. The last 4 itmes are placeholders and they are not used. |
| `Keyboard.effort` | `double` | The typing effort of the Keyboard. It is calculated based on Colemak-DH's typing effort model. However, the typing effort model is scaled down so that the average typing effort is `1.0`. This is done so that a randomly generated Keyboard will have a typing effort of `100.0` |
| `Keyboard.hand_diff` | `double` | The typing effort difference of the Left Hand and the Right Hand. |
| `Keyboard.sfb` | `double` | Stands for Same Finger Bigram. Measures the frequency of Bigrams typed using the same Fingers. |
| `Keyboard.lsb` | `double` | Stands for Lateral Stretch Bigram. Measures the frequency of Bigrams typed that require lateral stretching (any of column combinations 2, 4 or 5, 7). |
| `Keyboard.intwist` | `double` |  |
| `Keyboard.outtwist` | `double` |  |
| `Keyboard.total_twists` | `double` |  |
| `Keyboard.outtwist_ratio` | `double` |  |
| `Keyboard.alternate` | `double` |  |
| `Keyboard.inroll` | `double` |  |
| `Keyboard.outroll` | `double` |  |
| `Keyboard.total_rolls` | `double` |  |
| `Keyboard.inroll_ratio` | `double` |  |
| `Keyboard.onehand` | `double` |  |
| `Keyboard.redirect` | `double` |  |

### Keyboard.class_methods()

# Documentations (analyzer.py)

### Functions
