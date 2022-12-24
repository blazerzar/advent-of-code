use std::collections::{HashSet, LinkedList};
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

    let grid = read_file(&input_path);
    println!("{}", first_puzzle(&grid));
    println!("{}", second_puzzle(&grid));
}

fn read_file(path: &str) -> Vec<Vec<u8>> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| line.as_bytes().to_vec())
        .collect()
}

fn test(test_input_path: &str) {
    let grid = read_file(test_input_path);
    assert_eq!(first_puzzle(&grid), 18);
    assert_eq!(second_puzzle(&grid), 54);
}

type Blizzard = HashSet<(usize, usize)>;
type Blizzards = (Blizzard, Blizzard, Blizzard, Blizzard);

fn get_blizzards(grid: &[Vec<u8>]) -> Blizzards {
    let mut blizzards_up = HashSet::new();
    let mut blizzards_left = HashSet::new();
    let mut blizzards_right = HashSet::new();
    let mut blizzards_down = HashSet::new();

    for (y, line) in grid.iter().enumerate() {
        for (x, cell) in line.iter().enumerate() {
            match cell {
                b'^' => blizzards_up.insert((x, y)),
                b'v' => blizzards_down.insert((x, y)),
                b'<' => blizzards_left.insert((x, y)),
                b'>' => blizzards_right.insert((x, y)),
                _ => true,
            };
        }
    }

    (
        blizzards_up,
        blizzards_left,
        blizzards_right,
        blizzards_down,
    )
}

const DIRECTIONS: [(isize, isize); 5] = [(0, -1), (-1, 0), (1, 0), (0, 1), (0, 0)];

fn update_blizzard(blizzard: &Blizzard, direction: usize, height: usize, width: usize) -> Blizzard {
    let mut moved_blizzard = HashSet::new();
    let (dx, dy) = DIRECTIONS[direction];
    let width = width as isize - 2;
    let height = height as isize - 2;

    for &(x, y) in blizzard {
        let (x_, y_) = (x as isize + dx, y as isize + dy);
        let x_ = ((x_ - 1) % width + width) % width + 1;
        let y_ = ((y_ - 1) % height + height) % height + 1;
        moved_blizzard.insert((x_ as usize, y_ as usize));
    }

    moved_blizzard
}

fn update_blizzards(blizzards: &Blizzards, height: usize, width: usize) -> Blizzards {
    (
        update_blizzard(&blizzards.0, 0, height, width),
        update_blizzard(&blizzards.1, 1, height, width),
        update_blizzard(&blizzards.2, 2, height, width),
        update_blizzard(&blizzards.3, 3, height, width),
    )
}

fn is_clear(blizzards: &Blizzards, x: usize, y: usize) -> bool {
    !blizzards.0.contains(&(x, y))
        && !blizzards.1.contains(&(x, y))
        && !blizzards.2.contains(&(x, y))
        && !blizzards.3.contains(&(x, y))
}

fn find_path(
    start: (usize, usize),
    goal: (usize, usize),
    mut blizzards: Blizzards,
    grid: &[Vec<u8>],
    height: usize,
    width: usize,
) -> (i32, Blizzards) {
    let mut visited_minute = 0;

    let mut visited = HashSet::new();
    let mut q = LinkedList::new();
    q.push_back((start.0, start.1, 0));
    visited.insert((start.0, start.1, 0));

    while let Some((x, y, minute)) = q.pop_front() {
        // Check if goal has been reached
        if x == goal.0 && y == goal.1 {
            return (minute, blizzards);
        }

        // Update blizzards, if new minute has been seen
        if minute != visited_minute {
            visited_minute = minute;
            blizzards = update_blizzards(&blizzards, height, width);
        }

        for (dx, dy) in DIRECTIONS {
            let (x_, y_) = (x as isize + dx, y as isize + dy);
            if y_ < 0 || x_ < 0 || y_ >= height as isize || x_ >= width as isize {
                continue;
            }

            let (x_, y_) = (x_ as usize, y_ as usize);
            if visited.contains(&(x_, y_, minute + 1)) {
                continue;
            }

            if grid[y_][x_] != b'#' && is_clear(&blizzards, x_, y_) {
                q.push_back((x_, y_, minute + 1));
                visited.insert((x_, y_, minute + 1));
            }
        }
    }

    panic!("Goal could not be reached!");
}

fn first_puzzle(grid: &[Vec<u8>]) -> i32 {
    let (height, width) = (grid.len(), grid[0].len());
    let start = (1, 0);
    let goal = (width - 2, height - 1);

    // Function find_path takes blizzards in the next minute
    let blizzards = get_blizzards(grid);
    let blizzards = update_blizzards(&blizzards, height, width);

    find_path(start, goal, blizzards, grid, height, width).0
}

fn second_puzzle(grid: &[Vec<u8>]) -> i32 {
    let (height, width) = (grid.len(), grid[0].len());
    let start = (1, 0);
    let goal = (width - 2, height - 1);
    let blizzards = get_blizzards(grid);
    let blizzards = update_blizzards(&blizzards, height, width);

    // Function find_path also returns blizzards in the next minute
    let (path_1, blizzards) = find_path(start, goal, blizzards, grid, height, width);
    let (path_2, blizzards) = find_path(goal, start, blizzards, grid, height, width);
    let (path_3, _) = find_path(start, goal, blizzards, grid, height, width);

    path_1 + path_2 + path_3
}
