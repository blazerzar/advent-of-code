use std::fs;
use std::path::PathBuf;

type Assignment = ((u32, u32), (u32, u32));

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

    let assignments = read_file(&input_path);
    println!("{}", first_puzzle(&assignments));
    println!("{}", second_puzzle(&assignments));
}

fn read_file(path: &str) -> Vec<Assignment> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let elves: Vec<Vec<u32>> = line
                .split(',')
                .map(|elve| {
                    elve.split('-')
                        .map(|section| section.parse().unwrap())
                        .collect()
                })
                .collect();
            ((elves[0][0], elves[0][1]), (elves[1][0], elves[1][1]))
        })
        .collect()
}

fn test(test_input_path: &str) {
    let assignments = read_file(test_input_path);
    assert!(first_puzzle(&assignments) == 2);
    assert!(second_puzzle(&assignments) == 4);
}

fn first_puzzle(assignments: &[Assignment]) -> u32 {
    assignments
        .iter()
        .map(|((a_begin, a_end), (b_begin, b_end))| {
            u32::from(a_begin <= b_begin && a_end >= b_end || b_begin <= a_begin && b_end >= a_end)
        })
        .sum()
}

fn second_puzzle(assignments: &[Assignment]) -> u32 {
    assignments
        .iter()
        .map(|((a_begin, a_end), (b_begin, b_end))| {
            u32::from(
                a_begin <= b_begin && b_begin <= a_end || b_begin <= a_begin && a_begin <= b_end,
            )
        })
        .sum()
}
