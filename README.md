# Semi-auto Keyboard Analyzer

## Documentations (analyzer.cpp)

You will use 2 json files to generate the keyboard layouts

### config.json

| Key | Description |
| --- | ----------- |
| `generation` | The number of generation you're working on |
| `iterations` | The number of keyboard layouts randomly generated in total |
| `remaining` | The keys that are not fixed nor constrained |
| `keyboard` | The keyboard with fixed keys |
| `constraints` | A dict of keys and their constrained locations (list[int]) |

### cutoff_score.json

gets anything **below** percentile of the keys  <br>
for example: `0.3` means only keep the best 30% in this criterion  <br>
the keyboard layout has to fulfill all the thresholds  <br>

After editing the .json files, you can run the program

The console will print the time elapsed when the program is done.

## Documentations (hash_table_writer.py)

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
  
It's uploaded to AKL discord. You can view the stats using `!bmini view colevrak` to view my keyboard layout
