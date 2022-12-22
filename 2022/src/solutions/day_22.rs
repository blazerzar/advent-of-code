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

    let (board, movement) = read_file(&input_path);
    println!("{}", first_puzzle(&board, &movement));
    println!("{}", second_puzzle(&board, &movement, 50));
}

type Board = Vec<Vec<u8>>;
enum Instruction {
    Move(usize),
    RotateClockwise,
    RotateCounterclockwise,
}

fn read_file(path: &str) -> (Board, Vec<Instruction>) {
    let input = fs::read_to_string(path).unwrap();
    let parts = input.split("\n\n").collect::<Vec<_>>();
    let board = parts[0]
        .split('\n')
        .map(|line| line.as_bytes().to_vec())
        .collect();

    let mut movement = Vec::new();
    for c in parts[1].trim().as_bytes() {
        match c {
            b'R' => movement.push(Instruction::RotateClockwise),
            b'L' => movement.push(Instruction::RotateCounterclockwise),
            _ => {
                if let Some(Instruction::Move(n)) = movement.last() {
                    let n = 10 * n + (c - b'0') as usize;
                    movement.pop();
                    movement.push(Instruction::Move(n));
                } else {
                    movement.push(Instruction::Move((c - b'0') as usize));
                }
            }
        }
    }

    (board, movement)
}

fn test(test_input_path: &str) {
    let (board, movement) = read_file(test_input_path);
    assert_eq!(first_puzzle(&board, &movement), 6032);
    // assert_eq!(second_puzzle(&board, &movement, 4), 5031);
}

const STEP: [(isize, isize); 4] = [(1, 0), (0, 1), (-1, 0), (0, -1)];

fn valid_tile(board: &Board, x: isize, y: isize) -> bool {
    if y < 0 || y >= board.len() as isize || x < 0 {
        return false;
    }

    let (x, y) = (x as usize, y as usize);
    if x >= board[y].len() || board[y][x] == b' ' {
        return false;
    }

    true
}

fn wrap(board: &Board, x: usize, y: usize, facing: usize) -> (usize, usize, usize) {
    let (mut x, mut y) = (x, y);
    let (dx, dy) = STEP[(facing + 2) % 4];

    loop {
        let (x_, y_) = (x as isize + dx, y as isize + dy);
        if !valid_tile(board, x_, y_) {
            break;
        }

        (x, y) = (x_ as usize, y_ as usize);
    }

    (x, y, facing)
}

fn step(
    board: &Board,
    x: usize,
    y: usize,
    facing: usize,
    cube: Option<usize>,
) -> (usize, usize, usize) {
    let (dx, dy) = STEP[facing];
    let (x_, y_) = (x as isize + dx, y as isize + dy);

    if valid_tile(board, x_, y_) {
        // If tile is valid, move to it if it empty
        let (x_, y_) = (x_ as usize, y_ as usize);
        if board[y_][x_] == b'.' {
            return (x_, y_, facing);
        }
    } else {
        // Wrap around if wrapped tile is empty
        let (x_, y_, facing_) = if let Some(n) = cube {
            wrap_cube(x, y, facing, n)
        } else {
            wrap(board, x, y, facing)
        };
        if board[y_][x_] == b'.' {
            return (x_, y_, facing_);
        }
    }

    (x, y, facing)
}

fn password(board: &Board, movement: &[Instruction], cube: Option<usize>) -> usize {
    let (mut y, mut facing) = (0, 0);
    let mut x = board[y]
        .iter()
        .enumerate()
        .filter(|&(_, tile)| *tile == b'.')
        .map(|(index, _)| index)
        .min()
        .unwrap();

    for instruction in movement {
        match instruction {
            Instruction::RotateClockwise => facing = (facing + 1) % 4,
            Instruction::RotateCounterclockwise => facing = (facing + 3) % 4,
            Instruction::Move(n) => {
                for _ in 0..*n {
                    (x, y, facing) = step(board, x, y, facing, cube);
                }
            }
        }
    }

    1000 * (y + 1) + 4 * (x + 1) + facing as usize
}

fn first_puzzle(board: &Board, movement: &[Instruction]) -> usize {
    password(board, movement, None)
}

fn wrap_cube(x: usize, y: usize, facing: usize, n: usize) -> (usize, usize, usize) {
    if y == 0 && x < 2 * n && facing == 3 {
        // From 1 to 6
        (0, 2 * n + x, 0)
    } else if y == 0 && x < 3 * n && facing == 3 {
        // From 2 to 6
        (x - 2 * n, 4 * n - 1, 3)
    } else if y < n && x == n && facing == 2 {
        // From 1 to 5
        (0, 3 * n - 1 - y, 0)
    } else if y < n && x == 3 * n - 1 && facing == 0 {
        // From 2 to 4
        (2 * n - 1, 3 * n - 1 - y, 2)
    } else if y == n - 1 && x < 3 * n && facing == 1 {
        // From 2 to 3
        (2 * n - 1, x - n, 2)
    } else if y < 2 * n && x == n && facing == 2 {
        // From 3 to 5
        (y - n, 2 * n, 1)
    } else if y < 2 * n && x == 2 * n - 1 && facing == 0 {
        // From 3 to 2
        (n + y, n - 1, 3)
    } else if y == 2 * n && x < n && facing == 3 {
        // From 5 to 3
        (n, n + x, 0)
    } else if y < 3 * n && x == 0 && facing == 2 {
        // From 5 to 1
        (n, 3 * n - 1 - y, 0)
    } else if y < 3 * n && x == 2 * n - 1 && facing == 0 {
        // From 4 to 2
        (3 * n - 1, 3 * n - 1 - y, 2)
    } else if y == 3 * n - 1 && x < 2 * n && facing == 1 {
        // From 4 to 6
        (n - 1, 2 * n + x, 2)
    } else if y < 4 * n && x == 0 && facing == 2 {
        // From 6 to 1
        (y - 2 * n, 0, 1)
    } else if y < 4 * n && x == n - 1 && facing == 0 {
        // From 6 to 4
        (y - 2 * n, 3 * n - 1, 3)
    } else if y == 4 * n - 1 && x < n && facing == 1 {
        // From 6 to 2
        (2 * n + x, 0, 1)
    } else {
        panic!("Invalid wrap!");
    }
}

fn second_puzzle(board: &Board, movement: &[Instruction], n: usize) -> usize {
    password(board, movement, Some(n))
}
