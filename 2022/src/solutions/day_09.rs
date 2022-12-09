use std::collections::HashSet;
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

    let moves = read_file(&input_path);
    println!("{}", first_puzzle(&moves));
    println!("{}", second_puzzle(&moves));
}

fn read_file(path: &str) -> Vec<(u8, u32)> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let steps = line[2..].parse().unwrap();
            (line.as_bytes()[0], steps)
        })
        .collect()
}

fn test(test_input_path: &str) {
    let moves = read_file(test_input_path);
    assert!(first_puzzle(&moves) == 13);
    let moves = read_file(&test_input_path[..].replace("test", "test_large"));
    assert!(second_puzzle(&moves) == 36);
}

fn move_tail(head: (i32, i32), tail: (i32, i32)) -> (i32, i32) {
    let (dx, dy): (i32, i32) = (head.0 - tail.0, head.1 - tail.1);
    if dx.pow(2) + dy.pow(2) >= 4 {
        let move_x = if dx != 0 { dx / dx.abs() } else { 0 };
        let move_y = if dy != 0 { dy / dy.abs() } else { 0 };
        (tail.0 + move_x, tail.1 + move_y)
    } else {
        tail
    }
}

fn simulate_moves(moves: &[(u8, u32)], knots: usize) -> usize {
    let mut knots = vec![(0, 0); knots];

    // Keep track of positions the tail has visited
    let mut visited = HashSet::new();
    visited.insert((0, 0));

    for (direction, steps) in moves {
        for _ in 0..*steps {
            let head = &mut knots[0];
            match direction {
                b'U' => *head = (head.0, head.1 + 1),
                b'D' => *head = (head.0, head.1 - 1),
                b'L' => *head = (head.0 + 1, head.1),
                b'R' => *head = (head.0 - 1, head.1),
                _ => panic!("Invalid move character!"),
            };

            // Move all the following knots
            (1..knots.len()).for_each(|knot_index| {
                knots[knot_index] = move_tail(knots[knot_index - 1], knots[knot_index]);
            });

            visited.insert(*knots.last().unwrap());
        }
    }

    visited.len()
}

fn first_puzzle(moves: &[(u8, u32)]) -> usize {
    simulate_moves(moves, 2)
}

fn second_puzzle(moves: &[(u8, u32)]) -> usize {
    simulate_moves(moves, 10)
}
