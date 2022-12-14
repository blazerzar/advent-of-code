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

    let scan = read_file(&input_path);
    let rocks = calculate_rocks(&scan);
    println!("{}", first_puzzle(&rocks));
    println!("{}", second_puzzle(&rocks));
}

fn read_file(path: &str) -> Vec<Vec<(i32, i32)>> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            line.split(" -> ")
                .map(|coordinates| {
                    let coordinates = coordinates
                        .split(',')
                        .map(|v| v.parse().unwrap())
                        .collect::<Vec<_>>();
                    (coordinates[0], coordinates[1])
                })
                .collect()
        })
        .collect()
}

fn calculate_rocks(scan: &[Vec<(i32, i32)>]) -> HashSet<(i32, i32)> {
    let mut rocks = HashSet::new();

    for path in scan {
        for (&(x_1, y_1), &(x_2, y_2)) in path.iter().zip(path[1..].iter()) {
            let (dx, dy) = (x_2 - x_1, y_2 - y_1);
            let dx = dx / if dx != 0 { dx.abs() } else { 1 };
            let dy = dy / if dy != 0 { dy.abs() } else { 1 };
            let (mut x, mut y) = (x_1, y_1);

            while x != x_2 || y != y_2 {
                rocks.insert((x, y));
                x += dx;
                y += dy;
            }
            rocks.insert((x, y));
        }
    }

    rocks
}

fn test(test_input_path: &str) {
    let scan = read_file(test_input_path);
    let rocks = calculate_rocks(&scan);
    assert_eq!(first_puzzle(&rocks), 24);
    assert_eq!(second_puzzle(&rocks), 93);
}

fn first_puzzle(rocks: &HashSet<(i32, i32)>) -> usize {
    // Get lowest rock position to detect sand falling into void
    let min_y = rocks.iter().map(|&(_, y)| y).max().unwrap();
    let mut sand = HashSet::new();

    loop {
        // Simulate one sand unit
        let (mut x, mut y) = (500, 0);
        loop {
            if y > min_y {
                return sand.len();
            }

            if !rocks.contains(&(x, y + 1)) && !sand.contains(&(x, y + 1)) {
                y += 1;
            } else if !rocks.contains(&(x - 1, y + 1)) && !sand.contains(&(x - 1, y + 1)) {
                x -= 1;
                y += 1
            } else if !rocks.contains(&(x + 1, y + 1)) && !sand.contains(&(x + 1, y + 1)) {
                x += 1;
                y += 1;
            } else {
                sand.insert((x, y));
                break;
            }
        }
    }
}

fn second_puzzle(rocks: &HashSet<(i32, i32)>) -> usize {
    // Floor instead of infinite void
    let floor = rocks.iter().map(|&(_, y)| y).max().unwrap() + 2;
    let mut sand = HashSet::new();

    loop {
        // Simulate one sand unit
        let (mut x, mut y) = (500, 0);
        loop {
            if !rocks.contains(&(x, y + 1)) && !sand.contains(&(x, y + 1)) && y + 1 < floor {
                y += 1;
            } else if !rocks.contains(&(x - 1, y + 1))
                && !sand.contains(&(x - 1, y + 1))
                && y + 1 < floor
            {
                x -= 1;
                y += 1
            } else if !rocks.contains(&(x + 1, y + 1))
                && !sand.contains(&(x + 1, y + 1))
                && y + 1 < floor
            {
                x += 1;
                y += 1;
            } else {
                sand.insert((x, y));

                if (x, y) == (500, 0) {
                    return sand.len();
                }

                break;
            }
        }
    }
}
