import json
import numpy as np
import os
import re
from collections import OrderedDict
from itertools import chain
from typing import List, Dict


def keyboard_effort_table():
    kb_effort: List[float] = [3.0, 2.5, 2.1, 2.3, 2.6, 3.4, 2.2, 2.0, 2.4, 3.0,
                              1.6, 1.3, 1.1, 1.0, 2.9, 2.9, 1.0, 1.1, 1.3, 1.6,
                              2.7, 2.4, 1.8, 2.2, 3.7, 2.2, 1.8, 2.4, 2.7, 3.3]
    total = sum(kb_effort)
    kb_effort = [i / total * 30 for i in kb_effort]
    print("Average: ", end="")
    print(float(sum(kb_effort) / len(kb_effort)))

    effort_table: dict = {}
    for v, i in enumerate(kb_effort):
        effort_table[f"key{v}"] = i
    effort_table_sorted = dict(sorted(effort_table.items(),
                                      key=lambda item: int("".join(re.findall("[0-9]+", item[0])))))
    effort_table_json = json.dumps(effort_table_sorted, indent=4)
    print(effort_table_sorted)
    with open("keyboard_effort_table1.json", "w") as f:
        f.write(effort_table_json)


def make_hash_table(filename: str, chunk_width: int, mode: str, include_blanks: bool = False):
    if mode not in ("row", "col", "hand", "key"):
        raise ValueError("mode must be in: row, col, hand, key")

    line_num: int
    line_text: str
    hash_table: Dict[str, dict] = {}
    sub_hash_table_num = {"row": 3, "col": 10, "hand": 2, "key": 30}
    for i in range(sub_hash_table_num[mode]):
        hash_table[f"{mode}{i}"] = {}

    for line_num, line_text in enumerate(open(filename, "r")):
        if line_num % chunk_width == 0:
            keys_text = line_text.split(": ")[1].strip()
            for v, c in enumerate(keys_text):
                pos_mode = {"row": v // 10, "col": v % 10, "hand": v % 10 // 5, "key": v}
                if not include_blanks and c == "-":
                    continue
                key_name = f"{mode}{pos_mode[mode]}"
                if c not in hash_table[key_name]:
                    hash_table[key_name][c] = 1
                else:
                    hash_table[key_name][c] += 1
    hash_table_sorted = {}
    for i in range(sub_hash_table_num[mode]):
        hash_table_sorted[f"{mode}{i}"] = dict(sorted(hash_table[f"{mode}{i}"].items(),
                                                      key=lambda item: item[1], reverse=True))
    hash_table_json = json.dumps(hash_table_sorted, indent=4)
    with open(f"{os.path.splitext(filename)[0]}_hash_table_{mode}"
              f"{'_with_blanks' if include_blanks else ''}.json", "w") as f:
        f.write(hash_table_json)


def make_letter_hash_table_col(gen: int):
    line_num: int
    line_text: str
    col_hash_table: dict[str, dict[str, int]] = dict((f"col{i}", {}) for i in range(10))
    for line_num, line_text in enumerate(open(f"gen{gen}.txt", "r")):
        if line_num % 14 != 0:
            continue
        keyboard_layout: str = line_text.split(": ")[1].strip()
        for v, ch in enumerate(keyboard_layout):
            if ch == '-':
                continue
            if ch in col_hash_table[f"col{v % 10}"]:
                col_hash_table[f"col{v % 10}"][ch] += 1
            else:
                col_hash_table[f"col{v % 10}"][ch] = 1

    letter_hash_table: dict[str, list[int] | str] = {}
    for k, col_vals in col_hash_table.items():
        for ch in range(97, 123):
            freq: int = col_vals.get(chr(ch), 0)
            if chr(ch) in letter_hash_table:
                letter_hash_table[chr(ch)].append(freq)
            else:
                letter_hash_table[chr(ch)] = [freq]
    letter_hash_table = dict(sorted(letter_hash_table.items(),  # NoQA
                                    key=lambda itm: sum(sorted(itm[1], reverse=True)[:4]), reverse=True))  # NoQA
    letter_hash_table = dict((k, repr(col_vals)) for (k, col_vals) in letter_hash_table.items())
    with open(f"gen{gen}_letter_hash_table_col.json", "w") as f:
        json.dump(letter_hash_table, f, indent=4)


def key_position_hash_table(file_path, chunk_width: int):
    key_pos_hash_table: dict[str, dict] = {}
    for i in range(97, 123):
        key_pos_hash_table[chr(i)] = {}
    for line_num, line_text in enumerate(open(file_path)):
        if line_num % chunk_width == 0:
            line_text_split = line_text.split(": ")
            keys_text = "".join(re.findall("[a-z:-]", line_text_split[1]))
            for v, i in enumerate(keys_text):
                if i == "-":
                    continue
                if v not in key_pos_hash_table[i]:
                    key_pos_hash_table[i][v] = 1
                else:
                    key_pos_hash_table[i][v] += 1
    key_pos_hash_table_sorted = {}
    for i in range(97, 123):
        key_pos_hash_table_sorted[chr(i)] = dict(sorted(key_pos_hash_table[chr(i)].items(),
                                                        key=lambda item: item[1], reverse=True))
    key_pos_hash_map_json = json.dumps(key_pos_hash_table_sorted, indent=4)
    with open(f"{os.path.splitext(file_path)[0]}_key_pos_hash_table.json", "w") as f:
        f.write(key_pos_hash_map_json)


def hash_table_max_difference(file_path, mode: str):
    if mode not in ("row", "col", "hand"):
        raise ValueError("mode must be in: row, col, hand")
    total_items = {"row": 3, "col": 10, "hand": 2}

    with open(file_path, "r") as f:
        hash_table: Dict[str, Dict[str, int]] = json.load(f)
        difference_hash_table: dict = {}

        for i in range(97, 123):
            temp_val = [0] * total_items[mode]
            for item_num, (item_str, item_table) in enumerate(hash_table.items()):
                if chr(i) in item_table:
                    temp_val[item_num] = item_table[chr(i)]
            temp_val.sort(reverse=True)
            if temp_val[1]:
                difference_hash_table[chr(i)] = round((temp_val[0] - temp_val[1]) / temp_val[1], 5)
            else:
                difference_hash_table[chr(i)] = 69  # Nice

    difference_hash_table_sorted = dict(sorted(difference_hash_table.items(),
                                               key=lambda item: item[1], reverse=True))
    difference_hash_table_json = json.dumps(difference_hash_table_sorted, indent=4)
    with open(f"{os.path.splitext(file_path)[0]}_difference.json", "w") as f:
        f.write(difference_hash_table_json)


def make_hash_tables(gen: int):
    # for i in ("row", "col", "hand"):
    #     make_hash_table(f"gen{gen}.txt", 14, i)
    #     hash_table_max_difference(f"gen{gen}_hash_table_{i}.json", i)
    make_hash_table(f"gen{gen}.txt", 14, "row")
    hash_table_max_difference(f"gen{gen}_hash_table_row.json", "row")
    make_letter_hash_table_col(gen)


def best_keyboard(gen: int):
    line_text: str
    categories = {1: [], 2: [], 3: [], 4: [], 5: [], 6: [], 7: [], 8: [], 9: [],
                  10: [], 11: [], 12: [], 13: []}
    for line_num, line_text in enumerate(open(f"gen{gen}.txt", "r")):
        if line_num % 14 == 0:
            continue
        if line_num % 14 in (1, 2, 3, 4, 7, 10, 11):
            value = float(line_text.split(": ")[1])
            categories[line_num % 14].append(value)
    for k, i in categories.items():
        if k in (1, 2, 3, 4):
            i.sort()
        else:
            i.sort(reverse=True)

    ranking: dict = {}

    curr_kb_layout = ""
    temp_rank = 0
    for line_num, line_text in enumerate(open(f"gen{gen}.txt", "r")):
        if line_num % 14 == 0:
            curr_kb_layout = line_text
        if line_num % 14 in (1, 2, 3, 4, 7, 10, 11):
            value = float(line_text.split(": ")[1])
            temp_rank += categories[line_num % 14].index(value)
        if line_num % 14 == 13:
            ranking[curr_kb_layout.split(": ")[1].strip()] = temp_rank
            temp_rank = 0
    ranking_sorted = dict(sorted(ranking.items(), key=lambda item: item[1]))
    print(ranking_sorted)


def row_col_intersection(row: Dict[str, int], col: Dict[str, int | tuple[int, ...]]):
    intersection_table: dict = {}
    # Initialize Row
    for k, i in row.items():
        if isinstance(i, int):
            intersection_table[k] = list(range(10 * i, 10 * i + 10))
        if isinstance(i, tuple):
            intersection_table[k] = []
            for j in i:
                intersection_table[k].extend(list(range(10 * j, 10 * j + 10)))
    # init Col If not initialized Col, else remove some Col in Row
    for k, i in col.items():
        if isinstance(i, int):
            if k not in intersection_table:
                intersection_table[k] = list(range(i, i + 30, 10))
            else:
                j = len(intersection_table[k]) - 1
                while j >= 0:
                    if intersection_table[k][j] % 10 != i:
                        intersection_table[k].pop(j)
                    j -= 1
        if isinstance(i, tuple):
            if k not in intersection_table:
                intersection_table[k] = []
                for j in i:
                    intersection_table[k].extend(list(range(j, j + 30, 10)))
            else:
                j = len(intersection_table[k]) - 1
                while j >= 0:
                    if all(m != (intersection_table[k][j] % 10) for m in i):
                        intersection_table[k].pop(j)
                    j -= 1

    # Find fixed keys, pop the rest containing these keys
    fixed_keys: List[int] = []
    for k, i in intersection_table.items():
        if len(i) == 1:
            fixed_keys.append(i[0])
            continue
        i.sort()
    for i in intersection_table.values():
        if len(i) == 1:
            continue
        j = 0
        while j < len(i):
            if i[j] in fixed_keys:
                i.pop(j)
                continue
            j += 1

    # Sort the Table by the length of Lists
    intersection_table = dict(sorted(intersection_table.items(), key=lambda item: len(item[1])))

    # Print the Table
    for k, i in intersection_table.items():
        print(f"{k}: {i}")
    remaining_keys = ""
    for j in range(97, 123):
        if chr(j) not in intersection_table:
            remaining_keys += chr(j)
    print(remaining_keys)


if __name__ == '__main__':
    ...
    LHandLoc = tuple(range(0, 5))
    RHandLoc = tuple(range(5, 10))

    # keyboard_effort_table()
    # make_hash_tables(1)
    # best_keyboard(1)
    # Gen1row = dict(Item for Item in zip("eaoit", (1, 1, 1, 1, 1)))
    # Gen1col = dict(Item for Item in zip("enrhai", ((6, 7, 8, 9), (0, 1, 2, 3), (0, 1, 2, 3), (0, 2, 3, 9),
    #                                     (0, 2, 7, 8), (1, 2, 6, 7, 8))))
    # row_col_intersection(Gen1row, Gen1col)
    # make_hash_tables(2)
    # best_keyboard(2)
    # Gen2row = dict(Item for Item in zip("aeiotxn", (1, 1, 1, 1, 1, 2, 1)))
    # Gen2col = dict(Item for Item in zip("aehnrioltyum", ((0, 2, 7, 8), (6, 7, 8, 9), (0, 2, 3, 9), (0, 1, 2, 3),
    #                                                      (0, 1, 2, 3), (1, 2, 6, 7, 8), (1, 2, 6, 9), (0, 2, 3, 4),
    #                                                      (0, 2, 3, 9), (5, 6, 7, 8), (5, 7, 8, 9), (0, 1, 3, 4))))
    # row_col_intersection(Gen2row, Gen2col)
    # make_hash_tables(3)
    # best_keyboard(3)
    # Gen3row = dict(Item for Item in zip("aeiotxn", (1, 1, 1, 1, 1, 2, 1)))
    # Gen3col = dict(Item for Item in zip("ehlmruyioant", ((6, 7, 8), (2, 3, 9), (0, 2, 3, 4), (0, 1, 3, 4),
    #                                                      (0, 1, 2, 3), (5, 7, 8, 9), (5, 6, 7, 8), (6, 7, 8, 9),
    #                                                      (1, 2, 7), (1, 6, 7, 8, 9), (0, 2, 3, 7), (0, 1, 2, 3, 9))))
    # row_col_intersection(Gen3row, Gen3col)
    # make_hash_tables(4)
    # best_keyboard(4)
    # Gen4row = dict(Item for Item in zip("aeiotxn", (1, 1, 1, 1, 1, 2, 1)))
    # Gen4col = dict(Item for Item in zip("aehilmnorstuvy", ((6, 7, 8), (6, 7, 8), (3, 9), (6, 7, 8, 9),
    #                                                        (0, 2, 3, 4), (0, 1, 3, 4), (0, 2, 3),
    #                                                        (0, 1, 2, 3, 6, 7, 8, 9),
    #                                                        (0, 1, 2, 3), (0, 1, 2, 3, 6), (0, 1, 2, 3), (5, 7, 8, 9),
    #                                                        (0, 1, 2, 3, 4), (5, 6, 7, 8))))
    # row_col_intersection(Gen4row, Gen4col)
    # make_hash_tables(5)
    # best_keyboard(5)
    # Gen5row = dict(Item for Item in zip("aeiontxs", (1, 1, 1, 0, 1, 1, 2, 1)))
    # Gen5col = dict(Item for Item in zip("aehilmnorstuvy", ((6, 7, 8), (6, 7, 8), (0, 1, 2, 3), (6, 7, 8, 9),
    #                                                        (0, 2, 3, 4), (0, 1, 3, 4), (0, 2, 3), (7, 8),
    #                                                        (0, 1, 2, 3), (0, 1, 2, 3), (1, 2), (5, 7, 8, 9),
    #                                                        (0, 1, 2, 3, 4), (5, 6, 7, 8))))
    # row_col_intersection(Gen5row, Gen5col)
    # make_hash_tables(6)
    # best_keyboard(6)
    # Gen6row = dict(Item for Item in zip("aeionstxh", (1, 1, 1, 0, 1, 1, 1, 2, 1)))
    # Gen6col = dict(Item for Item in zip("aehilmnorstuvyc", ((7, 8), (7, 8), 9, (6, 7, 8),
    #                                                         (0, 2, 3, 4), (0, 1, 3, 4), (0, 1, 2, 3), (7, 8),
    #                                                         (0, 1, 2, 3), (0, 1, 2, 3), (1, 2, 3), (5, 6, 7, 8),
    #                                                         (0, 1, 2, 3, 4), (5, 6, 7, 8), (0, 1, 2, 3, 4))))
    # row_col_intersection(Gen6row, Gen6col)
    # make_hash_tables(7)
    # best_keyboard(7)
    # Gen7row = dict(Item for Item in zip("aehionrstux", (1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 2)))
    # Gen7col = dict(Item for Item in zip("aehilmnorstuvycd", (7, 8, 9, 6,
    #                                                          (0, 2, 3, 4), (0, 1, 3, 4), (0, 1, 2), (7, 8),
    #                                                          (0, 1, 2, 3), (0, 1, 2, 3), (1, 2, 3), (5, 6, 7, 8),
    #                                                          (0, 1, 2, 3, 4), (5, 6, 7, 8), (0, 1, 2, 3, 4),
    #                                                          (1, 2, 3, 4, 9))))
    # row_col_intersection(Gen7row, Gen7col)
    # make_hash_tables(8)
    # best_keyboard(8)
    # Gen8row = dict(Item for Item in zip("aehionrstuxy", (1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 2, 2)))
    # Gen8col = dict(Item for Item in zip("aehilmnorstuvycdw", (7, 8, 9, 6,
    #                                                           (0, 2, 3, 4), (0, 1, 3, 4), (0, 1, 2), 7,
    #                                                           (0, 1, 2, 3), (0, 1, 2, 3), (1, 2, 3), (6, 8),
    #                                                           (0, 1, 2, 3, 4), (5, 6), (0, 1, 2, 3, 4),
    #                                                           (1, 2, 3, 4, 9), (0, 1, 2, 3, 4))))
    # row_col_intersection(Gen8row, Gen8col)
    # make_hash_tables(9)
    # best_keyboard(9)
