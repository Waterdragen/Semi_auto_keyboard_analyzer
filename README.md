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
| `Keyboard.effort` | `double` | The typing effort of the Keyboard. |
| `` | `` | `` |
| `` | `` | `` |
| `` | `` | `` |
| `` | `` | `` |
| `` | `` | `` |
| `` | `` | `` |
| `` | `` | `` |
| `` | `` | `` |
