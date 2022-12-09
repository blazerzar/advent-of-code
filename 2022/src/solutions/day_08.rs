use std::collections::HashMap;
use std::fs;
use std::path::PathBuf;

pub fn solve() {
    let day = module_path!().split("::").last().unwrap();
    let day: u8 = day.split('_').last().unwrap().parse().unwrap();
    println!("Solving day {day}.");

    let input_path: PathBuf = ["inputs", &format!("day_{:02}.txt", day)].iter().collect();
    let test_input_path: PathBuf = ["inputs", &format!("day_{:02}_test.txt", day)]
        .iter()
        .collect();
    let input_path = input_path.display().to_string();
    let test_input_path = test_input_path.display().to_string();

    test(&test_input_path);

    let tree_grid = read_file(&input_path);
    println!("{}", first_puzzle(&tree_grid));
    println!("{}", second_puzzle(&tree_grid));
}

fn read_file(path: &str) -> Vec<Vec<u8>> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| line.as_bytes().iter().map(|char| char - b'0').collect())
        .collect()
}

fn test(test_input_path: &str) {
    let tree_grid = read_file(test_input_path);
    assert!(first_puzzle(&tree_grid) == 21);
    assert!(second_puzzle(&tree_grid) == 8);
}

fn first_puzzle(tree_grid: &[Vec<u8>]) -> u32 {
    let size = tree_grid.len();
    // Marks if tree (i, j) is visible
    let mut visible = vec![vec![false; size]; size];

    // Trees on the edge are always visible
    (0..size).for_each(|i| {
        (0..size).for_each(|j| {
            visible[i][j] = i == 0 || j == 0 || j == size - 1 || i == size - 1;
        })
    });

    // Calculate visibility for every direction
    for direction in 0..4 {
        (1..size - 1).for_each(|i| {
            let mut max_height = match direction {
                0 => tree_grid[0][i],
                1 => tree_grid[i][0],
                2 => tree_grid[i][size - 1],
                3 => tree_grid[size - 1][i],
                _ => panic!("Not possible"),
            };

            (1..size - 1).for_each(|j| {
                let (y, x) = match direction {
                    0 => (j, i),
                    1 => (i, j),
                    2 => (i, size - 1 - j),
                    3 => (size - 1 - j, i),
                    _ => panic!("Not possible"),
                };

                if tree_grid[y][x] > max_height {
                    visible[y][x] = true;
                    max_height = tree_grid[y][x];
                }
            })
        });
    }

    visible
        .iter()
        .map(|line| line.iter().map(|&tree| tree as u32).sum::<u32>())
        .sum()
}

fn second_puzzle(tree_grid: &[Vec<u8>]) -> usize {
    let size = tree_grid.len();
    // Marks how many trees tree (i, j) sees in all directions
    let mut visible = vec![vec![vec![0, 0, 0, 0]; size]; size];

    // Calculate visible trees for every direction
    for direction in 0..4 {
        (0..size).for_each(|i| {
            // Keep track of the last index for every tree height
            let mut last_seen = (0..10)
                .map(|height| {
                    (
                        height,
                        match direction {
                            0 | 1 => 0,
                            2 | 3 => size as isize - 1,
                            _ => panic!("Not possible"),
                        },
                    )
                })
                .collect::<HashMap<_, _>>();

            (0..size).for_each(|j| {
                let (y, x) = match direction {
                    0 => (j, i),
                    1 => (i, j),
                    2 => (i, size - 1 - j),
                    3 => (size - 1 - j, i),
                    _ => panic!("Not possible"),
                };

                let current_index = match direction {
                    0 | 1 => j,
                    2 | 3 => size - 1 - j,
                    _ => panic!("Not possible"),
                } as isize;

                // Find closest tree that is higher
                visible[y][x][direction] = (tree_grid[y][x]..10)
                    .map(|height| (last_seen.get(&height).unwrap() - current_index).unsigned_abs())
                    .min()
                    .unwrap();

                last_seen.insert(tree_grid[y][x], current_index);
            });
        });
    }

    visible
        .iter()
        .map(|line| {
            line.iter()
                .map(|tree| {
                    let mut product = 1;
                    tree.iter()
                        .for_each(|visible_trees| product *= visible_trees);
                    product
                })
                .max()
                .unwrap()
        })
        .max()
        .unwrap()
}
